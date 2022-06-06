#include "AutoPilotThread.h"
#include <loguru/loguru.hpp>
#include "../messages/messagetype.h"
#include "../messages/response/RespReqTrippoints.h"
#include <bsoncxx/builder/stream/document.hpp>

AutoPilotThread::AutoPilotThread(ConfigParams conf, std::shared_ptr<TCPSocket> outputSocket, int tr_id) : config(conf), Abstract_ThreadClass(1000, 200)
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
    ss << "SELECT latitude, longitude, rotation, height, FROM tr_basic_points WHERE idTrajet =" << tr_id << " AND isCheckpoint = true;";
    auto transaction4 = postgresConnection->createTransaction();
    return pqxx::stream_from::query(*(transaction4.get()), ss.str());
}

void AutoPilotThread::run()
{
    loguru::set_thread_name("Position Sender");
    // get all checkpoints
    LOG_F(INFO, "Start waiting for position request");
    std::string reqpointsmessage;
    nlohmann::json reqpointsdoc;
    // waiting for reqTRPoints
    do
    {
        reqpointsmessage = dataOutput->receiveMessage();
        reqpointsdoc = getJSONFromRequest(reqpointsmessage);
    } while (reqpointsdoc["requestType"] != REQUESTTYPES[MESSAGE_TYPE::REQ_TR_POINTS]);

    auto stream = getPointsFromPG(tripid);
    sendAllTrip(stream);

    stringstream name;
    name << "trip_" << tripid;
    auto db = mongodbConnection->getDatabase();
    // loop to send data
    std::string message;
    nlohmann::json doc;

    // get les id de points checkpoints

    stringstream ss;
    ss << "SELECT Idpoint AS nbCheckpoints FROM tr_basic_points WHERE Idtrajet =" << tripid << " AND isCheckpoint = true;";
    auto transaction4 = postgresConnection->createTransaction();
    pqxx::stream_from nbCheckpointsStream = pqxx::stream_from::query(*(transaction4.get()), ss.str());
    tuple<int> value;

    while (isRunFlag() && nbCheckpointsStream >> value)
    {
        usleep(task_period);
        try
        {
            // attendre la demande du serveur central
            do
            {
                message = dataOutput->receiveMessage();
                doc = getJSONFromRequest(message);
            } while (doc["requestType"] != REQUESTTYPES[MESSAGE_TYPE::NEXTDRONEPOSITION]);

            int idCheckpoint = std::get<0>(value);

            std::string value;
            nlohmann::json jsonvalue;
            do
            {
                value = dataOutput->receiveMessage();
                jsonvalue = getJSONFromRequest(value);
            } while (jsonvalue["requestType"] != REQUESTTYPES[MESSAGE_TYPE::RESP_DRONEPOSITION]);

            // get image avec le tr_id
            auto response = db.collection(name.str()).find_one(bsoncxx::builder::stream::document{} << "id_pos" << idCheckpoint << bsoncxx::builder::stream::finalize);
            auto jsonresp = bsoncxx::to_json(*response);

            auto strimage = jsonresp;
            auto jsonImage =  nlohmann::json::parse(strimage);
            auto image = jsonImage["image"];
            //  creer la trame
            dataOutput->sendMessage(image);
            //  envoyer la trame
        }
        catch (const std::exception &e)
        {
            LOG_F(ERROR, e.what());
        }
    }
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

void AutoPilotThread::sendAllTrip(pqxx::stream_from &stream)
{
    // create json message
    nlohmann::json message = {
        {"requestType", REQUESTTYPES[MESSAGE_TYPE::TR_FILE]}};
    std::tuple<int, int, int, int> row;
    nlohmann::json result = nlohmann::json::array();
    while (stream >> row)
    {
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

    // send that message is ready
    auto respTrpoint = new respTripPoints(message.dump().size());
    nlohmann::json resp = convertRespReqTripPoints(respTrpoint);

    dataOutput->sendMessage(resp.dump());
    std::string waitfilemessage;
    nlohmann::json doc;
    do
    {
        waitfilemessage = dataOutput->receiveMessage();
        doc = getJSONFromRequest(waitfilemessage);
    } while (doc["responseType"] != REQUESTTYPES[MESSAGE_TYPE::WAIT_TR_FILE]);

    dataOutput->sendMessage(message.dump());

    std::string trfileresp;
    nlohmann::json jsonresp;
    do
    {
        trfileresp = dataOutput->receiveMessage();
        jsonresp = getJSONFromRequest(trfileresp);
    } while (jsonresp["responseType"] != REQUESTTYPES[MESSAGE_TYPE::RESP_TR_FILE]);
}

nlohmann::json AutoPilotThread::convertRespReqTripPoints(respTripPoints *response)
{
    nlohmann::json document;
    document["responseType"] = REQUESTTYPES[response->ResponseType];
    document["fileSize"] = response->fileSize;
    return document;
}
