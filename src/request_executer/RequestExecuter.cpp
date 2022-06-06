#include "RequestExecuter.h"
#include "../messages/messagetype.h"
#include <loguru/loguru.hpp>
#include "../network/TCPSocket.h"
#include "../request_analyse/RequestAnalyser.h"
#include "../messages/response/dataRegisterResponse.h"
#include "../threads/AutoPilotThread.h"
#include "../messages/response/tripLaunchResponse.h"
#include "../messages/response/respErrorNotif.h"
#include "../messages/request/getOnePath.h"
#include "../messages/request/getPathList.h"

#include <iostream>

RequestExecuter::RequestExecuter(ConfigParams conf, std::shared_ptr<TCPSocket> inputSocket, std::shared_ptr<TCPSocket> outputSocket) : config(conf)
{
    dataInput = inputSocket;
    dataOutput = outputSocket;
    postgresConnection = make_unique<PostgresqlConnection>(config.postgres);
    mongodbConnection = make_unique<MongodbConnection>(config.mongoDb);
}

RequestExecuter::~RequestExecuter()
{
}

void RequestExecuter::executeRequest(Request *request)
{
    switch (request->RequestType)
    {
    case MESSAGE_TYPE::TR_SAVE:
    {
        TripSaveRequest *tripSaveRequest = static_cast<TripSaveRequest *>(request);
        executeTripSaveRequest(tripSaveRequest);
    }
    break;
    case MESSAGE_TYPE::TR_LAUNCH:
    {
        TripLaunchRequest *tripLaunchRequest = static_cast<TripLaunchRequest *>(request);
        executeTripLaunchRequest(tripLaunchRequest);
    }
    break;
    case MESSAGE_TYPE::REQUEST:
    {
        DataRequest *dataRequest = static_cast<DataRequest *>(request);
        executeDataRequest(dataRequest);
    }
    break;
    case MESSAGE_TYPE::REGISTER:
    {
        DroneDataRegister *dronedataRequest = static_cast<DroneDataRegister *>(request);
        registerNewPositionTrip(dronedataRequest->tr_id, dronedataRequest->pointId, dronedataRequest);
    }
    break;
    case MESSAGE_TYPE::END_TR_SAVE:
    {
        endTripSaveRequest *endTripSaveReq = static_cast<endTripSaveRequest *>(request);
        executeEndTripSaveRequest(endTripSaveReq);
    }
    break;
    case MESSAGE_TYPE::GET_PATH_LIST:
    {
        getPathList *pathListRequest = static_cast<getPathList *>(request);
    }
    break;
    case MESSAGE_TYPE::GET_ONE_PATH:
    {
        getOnePath *onePathRequest = static_cast<getOnePath *>(request);
    }
    break;
    default:
        break;
    }
}

void RequestExecuter::executeGetPathList(getPathList *pathListRequest)
{
    
}

void RequestExecuter::executeGetOnePath(getOnePath *onePathRequest)
{


}

void RequestExecuter::executeHistoricSave(Request *request, int idLaunch)
{
    DroneDataRegister *data = static_cast<DroneDataRegister *>(request);
    LOG_F(INFO, "Start saving a position as historic trip position for basic trip %d", data->tr_id);
    auto transaction1 = postgresConnection->createTransaction(); // instance of pqxx::work
    std::time_t result = std::time(nullptr);                     // TODO
    transaction1->exec_prepared(PostgresqlConnection::SAVE_HISTORIC_TRIP_POINT, idLaunch, data->tr_id, data->pointId, data->latitude, data->longitude, data->altitude, data->rotation, std::asctime(std::localtime(&result)), data->isCheckpoint);
    transaction1->commit();
}

// TODO
void RequestExecuter::executeDataRequest(DataRequest *dataRequest)
{
}

void RequestExecuter::executeDroneDataRegister(DroneDataRegister *dronedataRegister)
{
}

void RequestExecuter::executeTripLaunchRequest(TripLaunchRequest *tripLaunchRequest)
{
    LOG_F(INFO, "Launch a registered trip ... ");

    int idLaunch = getNewHistoricTripId();
    registerNewHistoricTripEntry(idLaunch, tripLaunchRequest->tr_id);

    auto launchresp = TripLaunchResponse(true);
    auto strlaunchresp = converter.convertToSendRequest(&launchresp);
    dataInput->sendMessage(strlaunchresp.dump());
    RequestAnalyser analyser(config, dataInput, dataOutput);

    // create the thread & launch it
    auto thread = make_unique<AutoPilotThread>(config, dataOutput, tripLaunchRequest->tr_id);
    thread->start();
    // await for positions and save them OR for end_tr_save OR ERROR & END_ERROR
    bool isError = false;
    int nbRegisterDuringError = 0;
    int pointId = 0;

    while (thread->isRunFlag())
    {
        auto message = dataInput->receiveMessage();
        auto jsonmessage = analyser.getJSONFromRequest(message);
        string reqType = jsonmessage["requestType"];
        MESSAGE_TYPE messageType = analyser.messagesTypeMap[reqType];
        auto transaction = postgresConnection->createTransaction(); // instance of pqxx::work
        switch (messageType)
        {
        case MESSAGE_TYPE::REGISTER:
        {
            if (isError)
            {
                nbRegisterDuringError = nbRegisterDuringError + 1;
            }
            pointId += 1;
            DroneDataRegister *datareq = analyser.parseDroneDataRequest(tripLaunchRequest->tr_id, pointId, jsonmessage);
            registerNewPositionTrip(tripLaunchRequest->tr_id, pointId, datareq, true, idLaunch);
        }
        // methode de register pour save dans la BD /!\ -> bool pour savoir si on est en erreur ou pas
        break;
        case MESSAGE_TYPE::TR_ERROR:
        {
            isError = true;
            // envoi RESP
            auto resp = respErrorNotif();
            auto jsonresp = converter.convertToSendRequest(&resp);
            dataInput->sendMessage(jsonresp);
            nbRegisterDuringError = 0;
        }
        break;
        case MESSAGE_TYPE::END_TR_ERROR:
        {
            isError = false;
            // save de la fin de l'erreur /!\ utiliser nb register pour savoir si on a pris la main ou pas
            bool action = false;
            int startingpoint;
            int endingpoint = pointId;
            if (nbRegisterDuringError > 0)
            {
                isError = true;
                startingpoint = endingpoint - nbRegisterDuringError;
            }

            std::time_t nowtime = std::time(nullptr);
            transaction->exec_prepared(PostgresqlConnection::SAVE_ERROR_DATA, idLaunch, tripLaunchRequest->tr_id, startingpoint, action, std::asctime(std::localtime(&nowtime)), startingpoint, endingpoint);
            transaction->commit();

            auto resp = respErrorNotif();
            auto jsonresp = converter.convertToSendRequest(&resp);
            dataInput->sendMessage(jsonresp);

            // envoi RESP
        }
        break;

        default:
            break;
        }
    }

    thread->join();
}

void RequestExecuter::executeTripSaveRequest(TripSaveRequest *TripSaveRequest)
{
    LOG_F(INFO, "Start saving a trip... ");

    LOG_F(INFO, "Requesting Data on the databases ... ");
    int tripId = getNewTripId();
    registerNewTripEntry(tripId);

    // alerting user that we are ready to save
    StartTripSaveResponse response = StartTripSaveResponse(true);
    nlohmann::json responseJson = converter.convertToSendRequest(&response);
    dataInput->sendMessage(responseJson.dump());

    RequestAnalyser analyser(config, dataInput, dataOutput);
    bool isRunning = true;
    int pointId = 0;
    while (isRunning)
    {
        LOG_F(INFO, "Waiting for a drone data to save or a END_TR_SAVE request ... ");

        auto buffer = dataInput->receiveMessage();
        LOG_F(INFO, "buffersize : %d", buffer.size());
        isRunning = analyser.parseSaveRequest(buffer, tripId, pointId);
        LOG_F(INFO, "isRunning %d", isRunning);
        pointId += 1;
    }

    LOG_F(INFO, "End of trip saving");
}

void RequestExecuter::registerImage(int tripId, int positionId, std::vector<uint8_t> image, int idLaunch)
{
    LOG_F(INFO, "Start saving image ... ");
    auto database = mongodbConnection->getDatabase();

    // bool hasCollection = database.has_collection("trip_"+to_string(tripId) );
    stringstream ss;
    ss << "trip_" << tripId;
    if (idLaunch != -1)
    {
        ss << "_idLaunch_" << idLaunch;
    }
    auto collection = database[ss.str()];
    // auto bulkOp = collection.create_bulk_write();
    // uint8_t* test = {(uint8_t*)malloc(sizeof(uint8_t)*10);
    bsoncxx::builder::basic::array arrrayBuilder{};

    bsoncxx::array::view a{image.data(), image.size()};
    for (auto const &value : image)
    {
        arrrayBuilder.append(value);
    }
    auto array = arrrayBuilder.view();
    // ==== Example insertion ====
    // Create the document (a json object)
    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value doc_value = builder
                                         << "id_pos" << positionId
                                         << "width" << 640
                                         << "height" << 480
                                         << "image" << array
                                         << bsoncxx::builder::stream::finalize; // builder.build()

    // insert it
    auto start = std::chrono::steady_clock::now();
    mongocxx::options::insert insertOpts;
    insertOpts.bypass_document_validation(true);
    collection.insert_one(doc_value.view(), insertOpts);
    auto end = std::chrono::steady_clock::now();
    std::cout << "Time for one insertion : " << std::chrono::duration_cast<chrono::milliseconds>(end - start).count();
    LOG_F(INFO, "Stop saving image");
}

void RequestExecuter::registerNewPositionTrip(int tripId, int pointId, DroneDataRegister *data, bool isHistoric, int idLaunch)
{
    LOG_F(INFO, "Start saving a position as basic trip position for trip %d", tripId);
    auto transaction1 = postgresConnection->createTransaction(); // instance of pqxx::work
    std::time_t result = std::time(nullptr);
    if (isHistoric)
    {
        transaction1->exec_prepared(PostgresqlConnection::SAVE_HISTORIC_TRIP_POINT, idLaunch, tripId, pointId, data->latitude, data->longitude, data->altitude, data->rotation, std::asctime(std::localtime(&result)), data->isCheckpoint);
    }
    else
    {
        transaction1->exec_prepared(PostgresqlConnection::SAVE_BASIC_TRIP_POINT, tripId, pointId, data->latitude, data->longitude, data->altitude, data->rotation, std::asctime(std::localtime(&result)), data->isCheckpoint);
    }
    transaction1->commit();

    LOG_F(INFO, "Save pxdata for trip %d", tripId);
    auto transaction2 = postgresConnection->createTransaction(); // instance of pqxx::work
    transaction2->exec_prepared(PostgresqlConnection::SAVE_TRIP_PXDATA, tripId, pointId, data->pressure, data->temperature, data->batteryRemaining);
    transaction2->commit();

    DataRegisterResponse resp = DataRegisterResponse(true);
    nlohmann::json doc = converter.convertToSendRequest(&resp);
    dataInput->sendMessage(doc.dump());
    LOG_F(INFO, "CHECKPOINT DETECTED : Waiting for image to save...");
    long imagesize = data->image;
    LOG_F(INFO, "image size : %ld", imagesize);
    auto image = dataInput->receiveBigMessage<unsigned char>(imagesize);
    // string str = str.assign(image.begin(), image.end());
    LOG_F(INFO, "image : %s", image);

    // image save
    if (image.size() != imagesize)
    {
        LOG_F(INFO, "received size : %ld --- awaited size : %ld", image.size(), imagesize);
        // throw runtime_error("size not valid !");
    }
    registerImage(tripId, pointId, image);
}

void RequestExecuter::executeEndTripSaveRequest(endTripSaveRequest *endTripSaveRequest)
{
    LOG_F(INFO, "Send to client that the trip is saved");
    EndTripSaveResponse response = EndTripSaveResponse(true);
    nlohmann::json responseJson = converter.convertToSendRequest(&response);
    dataInput->sendMessage(responseJson.dump());
}

int RequestExecuter::getNewTripId()
{
    auto transaction1 = postgresConnection->createTransaction(); // instance of pqxx::work
    pqxx::result result(transaction1->exec("SELECT count(*) FROM tr_basic;"));
    auto row = result[0];
    int nbtrips;
    auto opt = row["count"].get<int>();
    if (opt)
    {
        nbtrips = *opt;
    }
    LOG_F(INFO, "current number of trip registered : %d", nbtrips);
    int tripNumber = nbtrips++;
    transaction1->commit();
    return tripNumber;
}

int RequestExecuter::getNewHistoricTripId()
{
    auto transaction1 = postgresConnection->createTransaction(); // instance of pqxx::work
    pqxx::result result(transaction1->exec("SELECT count(*) FROM tr_historic;"));
    auto row = result[0];
    int nbtrips;
    auto opt = row["count"].get<int>();
    if (opt)
    {
        nbtrips = *opt;
    }
    LOG_F(INFO, "current number of trip registered : %d", nbtrips);
    int tripNumber = nbtrips++;
    transaction1->commit();
    return tripNumber;
}

void RequestExecuter::registerNewTripEntry(int tripId)
{
    LOG_F(INFO, "Saving new trip entry in basic trip List");
    auto transaction1 = postgresConnection->createTransaction(); // instance of pqxx::work
    std::time_t result = std::time(nullptr);
    transaction1->exec_prepared(PostgresqlConnection::SAVE_TRIP_ENTRY, tripId, "undefined Name", std::asctime(std::localtime(&result)));
    transaction1->commit();
}

void RequestExecuter::registerNewHistoricTripEntry(int tripId, int basicTripId)
{
    LOG_F(INFO, "Saving new trip entry in historic trip List");
    auto transaction1 = postgresConnection->createTransaction(); // instance of pqxx::work
    std::time_t result = std::time(nullptr);
    transaction1->exec_prepared(PostgresqlConnection::SAVE_HISTORIC_TRIP_ENTRY, tripId, basicTripId, "undefined Name", std::asctime(std::localtime(&result)));
    transaction1->commit();
}