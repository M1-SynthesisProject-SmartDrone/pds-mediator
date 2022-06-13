#include "AutoPilotThread.h"
#include <loguru/loguru.hpp>
#include "../messages/messagetype.h"
#include "../messages/response/RespReqTrippoints.h"
#include <bsoncxx/builder/stream/document.hpp>
#include "../messages/response/dronePosition.h"

AutoPilotThread::AutoPilotThread(ConfigParams conf, std::shared_ptr<TCPSocket> outputSocket, int tr_id) : Abstract_ThreadClass("DroneSender",1000, 200), config(conf)
{
    postgresConnection = make_unique<PostgresqlConnection>(config.postgres);
    mongodbConnection = make_unique<MongodbConnection>(config.mongoDb);
    dataOutput = outputSocket;
    tripid = tr_id;
}

AutoPilotThread::~AutoPilotThread()
{
}

pqxx::stream_from AutoPilotThread::getPointsFromPG(int tr_id)
{
    // auto transaction3 = postgresConnection->createTransaction();
    // string query = "SELECT latitude, longitude, rotation, height, FROM tr_basic_points WHERE idTrajet = $1 AND isCheckpoint = true";
    // return make_unique<pqxx::stateless_cursor<pqxx::cursor_base::read_only, pqxx::cursor_base::owned>>(*(transaction3.get()), query, "myCursor", false);
    stringstream ss;
    ss << "SELECT latitude, longitude, rotation, height FROM tr_basic_points WHERE idTrajet =" << tr_id << " AND isCheckpoint = true";
    auto transaction4 = postgresConnection->createTransaction();
    return pqxx::stream_from::query(*(transaction4.get()), ss.str());
}

void AutoPilotThread::run()
{
    initRun();
    // get all checkpoints
    LOG_F(INFO, "Start waiting for position request");
    std::string reqpointsmessage;
    nlohmann::json reqpointsdoc;
    // waiting for reqTRPoints
    do
    {
        reqpointsmessage = dataOutput->receiveMessage();
        reqpointsdoc = getJSONFromRequest(reqpointsmessage);
    } while (reqpointsdoc["requestType"] != REQUESTTYPES[MESSAGE_TYPE::REQ_TR_POINTS].c_str());
    // auto stream = getPointsFromPG(tripid);

    sendAllTrip(tripid);
    LOG_F(INFO, "out of sendAllTrip");

    stringstream name;
    name << "trip_" << tripid;
    auto db = mongodbConnection->getDatabase();
    // loop to send data
    std::string message;
    nlohmann::json doc;

    // get les id de points checkpoints

    stringstream ss;
    ss << "SELECT Idpoint AS nbCheckpoints FROM tr_basic_points WHERE Idtrajet =" << tripid << " AND isCheckpoint = true";
    auto transaction4 = postgresConnection->createTransaction();
    pqxx::stream_from nbCheckpointsStream = pqxx::stream_from::query(*(transaction4.get()), ss.str());
    tuple<int> value;

    while ( nbCheckpointsStream >> value)
    {
        usleep(task_period);
        try
        {
            // attendre la demande du serveur central
            LOG_F(INFO, "await for NEXTDRONEPOSITION");
            do
            {
                message ="";
                message = dataOutput->receiveMessage();
                doc = getJSONFromRequest(message);
            } while (doc["requestType"] != REQUESTTYPES[MESSAGE_TYPE::NEXTDRONEPOSITION].c_str());

            int idCheckpoint = std::get<0>(value);

            // get Image
             // get image avec le tr_id
            auto response = db.collection(name.str()).find_one(bsoncxx::builder::stream::document{} << "id_pos" << idCheckpoint << bsoncxx::builder::stream::finalize);
            auto jsonresp = bsoncxx::to_json(*response);
            
            std::string strimage = jsonresp;
            auto jsonImage = nlohmann::json::parse(strimage);
            auto image = jsonImage["image"].dump();
            // add send DRONEPOSITION
            auto dronepos = dronePosition( image.size(),tripid);
            auto jsonedronepos = convertDronePosResponse(&dronepos);
            dataOutput->sendMessage(jsonedronepos.dump());
            std::string value;
            nlohmann::json jsonvalue;

            LOG_F(INFO, "await for RESP_DRONEPOSITION");
            do
            {
                value = dataOutput->receiveMessage();
                jsonvalue = getJSONFromRequest(value);
            } while (jsonvalue["requestType"] != REQUESTTYPES[MESSAGE_TYPE::RESP_DRONEPOSITION].c_str());

            // get image avec le tr_id
            // auto response = db.collection(name.str()).find_one(bsoncxx::builder::stream::document{} << "id_pos" << idCheckpoint << bsoncxx::builder::stream::finalize);
            // auto jsonresp = bsoncxx::to_json(*response);

            // auto strimage = jsonresp;
            // auto jsonImage = nlohmann::json::parse(strimage);
            // auto image = jsonImage["image"].dump();
            //  creer la trame
            LOG_F(INFO, "Send Image...");
            dataOutput->sendMessage(image);
            LOG_F(INFO, "Send Image finished");
            //  envoyer la trame
        }
        catch (const std::exception &e)
        {
            LOG_F(ERROR, e.what());
        }
    }
    transaction4->commit();
    nbCheckpointsStream.complete();
    LOG_F(INFO,"----- END OF SECONDARY THREAD");
}

nlohmann::json AutoPilotThread::getJSONFromRequest(string request)
{
    LOG_F(INFO, "Parsing received request");
    LOG_F(INFO, "json :%s", request.c_str());
    nlohmann::json json = nlohmann::json::parse(request);
    if (!json["requestType"].is_string())
    {
        throw invalid_argument("No requestType object found");
    }
    LOG_F(INFO, "Parsing received request done !");
    return json;
}

nlohmann::json AutoPilotThread::convertDronePosResponse(dronePosition* response){
    nlohmann::json document;
    document["responseType"] = REQUESTTYPES[response->ResponseType];
    document["id_pos"] =response->id_pos;
    document["imagesize"] =response->imagesize;
    return document;
}


void AutoPilotThread::sendAllTrip(int tr_id)
{
    // create json message
    stringstream ss;
    ss << "SELECT latitude, longitude, rotation, height FROM tr_basic_points WHERE idTrajet =" << tr_id << " AND isCheckpoint = true";
    auto transaction4 = postgresConnection->createTransaction();
    auto stream = pqxx::stream_from::query(*(transaction4.get()), ss.str());

    LOG_F(INFO, "Starting sendAllTrip method");
    nlohmann::json message = {
        {"responseType", REQUESTTYPES[MESSAGE_TYPE::TR_FILE]}};
    std::tuple<int, int, int, int> row;
    nlohmann::json result = nlohmann::json::array();
    LOG_F(INFO, "Reading stream to get points ");

    while (stream >> row)
    {
        LOG_F(INFO, "new line read");
        // create document
        nlohmann::json j = {
            {"lat", std::get<0>(row)},
            {"lon", std::get<1>(row)},
            {"height", std::get<2>(row)},
            {"rotation", std::get<3>(row)}

        };

        result.push_back(j);
    }
    message["content"] = result;
    LOG_F(INFO, "End of stream");

    // send that message is ready
    auto respTrpoint = new respTripPoints(message.dump().size());
    nlohmann::json resp = convertRespReqTripPoints(respTrpoint);
    LOG_F(INFO, "End of convertRespReqTripPoints");

    dataOutput->sendMessage(resp.dump());
    LOG_F(INFO, "json sent");

    std::string waitfilemessage;
    nlohmann::json doc;
    do
    {
        waitfilemessage = dataOutput->receiveMessage();
        doc = getJSONFromRequest(waitfilemessage);
    } while (doc["requestType"] != REQUESTTYPES[MESSAGE_TYPE::WAIT_TR_FILE].c_str());
    LOG_F(INFO, "out of dowhile");

    dataOutput->sendMessage(message.dump());

    std::string trfileresp;
    nlohmann::json jsonresp;
    LOG_F(INFO, "await for resp_tr_file");
    do
    {
        trfileresp = dataOutput->receiveMessage();
        jsonresp = getJSONFromRequest(trfileresp);
    } while (jsonresp["requestType"] != REQUESTTYPES[MESSAGE_TYPE::RESP_TR_FILE].c_str());
}

nlohmann::json AutoPilotThread::convertRespReqTripPoints(respTripPoints *response)
{
    nlohmann::json document;
    document["responseType"] = REQUESTTYPES[response->ResponseType];
    document["fileSize"] = response->fileSize;
    return document;
}


