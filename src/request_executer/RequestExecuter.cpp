#include "RequestExecuter.h"
#include "../messages/messagetype.h"
#include <loguru/loguru.hpp>
#include "../network/TCPSocket.h"
#include "../request_analyse/RequestAnalyser.h"
#include "../messages/response/dataRegisterResponse.h"

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
        DroneDataRegister* dronedataRequest = static_cast<DroneDataRegister*>(request);
        registerNewPositionBasicTrip(dronedataRequest->tr_id, dronedataRequest->pointId, dronedataRequest);
    }
    break;
    case MESSAGE_TYPE::END_TR_SAVE:
    {
        endTripSaveRequest* endTripSaveReq = static_cast<endTripSaveRequest*>(request);
        executeEndTripSaveRequest(endTripSaveReq);
    }
    break;
    default:
        break;
    }
}

void RequestExecuter::executeHistoricSave(Request* request, int idLaunch){
    DroneDataRegister* data = static_cast<DroneDataRegister*>(request);
    LOG_F(INFO,"Start saving a position as historic trip position for basic trip %d", data->tr_id);
    auto transaction1 = postgresConnection->createTransaction(); // instance of pqxx::work
    std::time_t result = std::time(nullptr); // TODO
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

    int tripId = getNewHistoricTripId();
    registerNewHistoricTripEntry(tripId, tripLaunchRequest->tr_id);

    // create the thread & launch it
    // await for positions and save them OR for end_tr_save

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
    sockaddr_in sender;
    char* buffer = (char*) malloc(sizeof(char)*512);
    bool isRunning = true;
    int pointId = 0;
    while(isRunning){
        LOG_F(INFO, "Waiting for a drone data to save or a END_TR8SAVE request ... ");
        
        auto buffer = dataInput->receiveMessage();
        isRunning = analyser.parseSaveRequest(buffer, tripId, pointId);
        LOG_F(INFO, "isRunning %d", isRunning);
        pointId+=1;
    }

    LOG_F(INFO, "End of trip saving");
}

void RequestExecuter::registerImage(int tripId, int positionId, std::string image){
    LOG_F(INFO, "Start saving image ... ");
    auto database = mongodbConnection->getDatabase();
    bool hasCollection = database.has_collection("trip_"+to_string(tripId) );
    auto collection = database["trip_"+to_string(tripId)];

    // ==== Example insertion ====
    // Create the document (a json object)
    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value doc_value = builder
        << "id_pos" << positionId
        << "width" << 640
        << "height" << 480
        << "image" << image

    << bsoncxx::builder::stream::finalize; // builder.build()
    
    // insert it
    collection.insert_one(doc_value.view());
    LOG_F(INFO,"Stop saving image");
}

void RequestExecuter::registerNewPositionBasicTrip(int tripId, int pointId, DroneDataRegister* data){
    LOG_F(INFO,"Start saving a position as basic trip position for trip %d", tripId);
    auto transaction1 = postgresConnection->createTransaction(); // instance of pqxx::work
    std::time_t result = std::time(nullptr);
    transaction1->exec_prepared(PostgresqlConnection::SAVE_BASIC_TRIP_POINT, tripId, pointId, data->latitude, data->longitude, data->altitude, data->rotation, std::asctime(std::localtime(&result)), data->isCheckpoint);
    transaction1->commit();

    LOG_F(INFO,"Save pxdata for trip %d", tripId);
    auto transaction2 = postgresConnection->createTransaction(); // instance of pqxx::work
    transaction2->exec_prepared(PostgresqlConnection::SAVE_TRIP_PXDATA, tripId, pointId, data->pressure, data->temperature, data->batteryRemaining);
    transaction2->commit();

    DataRegisterResponse resp = DataRegisterResponse(true);
    nlohmann::json doc = converter.convertToSendRequest(&resp);
    dataInput->sendMessage(doc.dump());
    if(data->isCheckpoint){
        LOG_F(INFO, "CHECKPOINT DETECTED : Waiting for image to save...");
        long imagesize = data->image ;
        auto image = dataInput->receiveBigMessage<unsigned char>(imagesize);
        string str = str.assign(image.begin(), image.end());
        LOG_F(INFO,"image : %s", str);

        // image save
        registerImage(tripId, pointId, str);
    }
    
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
    LOG_F(INFO,"Saving new trip entry in basic trip List");
    auto transaction1 = postgresConnection->createTransaction(); // instance of pqxx::work
    std::time_t result = std::time(nullptr);
    transaction1->exec_prepared(PostgresqlConnection::SAVE_TRIP_ENTRY, tripId, "undefined Name", std::asctime(std::localtime(&result)) );
    transaction1->commit();
}


void RequestExecuter::registerNewHistoricTripEntry(int tripId, int basicTripId)
{
    LOG_F(INFO,"Saving new trip entry in historic trip List");
    auto transaction1 = postgresConnection->createTransaction(); // instance of pqxx::work
    std::time_t result = std::time(nullptr);
    transaction1->exec_prepared(PostgresqlConnection::SAVE_HISTORIC_TRIP_ENTRY, tripId, basicTripId, "undefined Name", std::asctime(std::localtime(&result)) );
    transaction1->commit();
}