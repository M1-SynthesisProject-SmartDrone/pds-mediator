/**
 * @file RequestAnalyser.cpp
 * @author Nicolas CIBULKA
 * @brief
 * @version 0.1
 * @date 2022-04-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "RequestAnalyser.h"
#include "config/config_parser.h"
#include <nlohmann/json.hpp>
#include <loguru/loguru.hpp>
#include "../messages/request/Request.h"
#include "../messages/messagetype.h"
#include "../messages/request/DataRequest.h"
#include "../messages/request/Request.h"
#include "../messages/request/TripSaveRequest.h"
#include "../messages/request/endTripSaveRequest.h"
#include "../request_executer/RequestExecuter.h"
#include "../network/TCPSocket.h"

using namespace std;
using namespace nlohmann;

RequestAnalyser::RequestAnalyser(ConfigParams conf, std::shared_ptr<TCPSocket> inputSocket, std::shared_ptr<TCPSocket> outputSocket) : config(conf), executer(conf, inputSocket, outputSocket)
{
}

RequestAnalyser::~RequestAnalyser()
{
}

/**
 * @brief It analyse the given request and execute the corresponding code.
 *
 * @param request
 */
void RequestAnalyser::parseRequest(string request)
{
    printf("request received :%s \n", request.c_str());
    nlohmann::json document = getJSONFromRequest(request);

    LOG_F(INFO, "tranform JSON into accurate object");
    Request *obj = getRequestFromDocument(document);

    executer.executeRequest(obj);
}

bool RequestAnalyser::parsePositionRequest(string request)
{
    LOG_F(INFO, "Request received during launch operation");
    nlohmann::json document = getJSONFromRequest(request);

    return document["requestType"] == "NEXTDRONEPOSITION";
}

bool RequestAnalyser::parseSaveRequest(std::string request, int tr_id, int pointId)
{
    LOG_F(INFO, "-> Request received during a save operation");
    nlohmann::json document = getJSONFromRequest(request);

    LOG_F(INFO, "tranform JSON into accurate object");

    Request *obj = getSaveRequestsFromDocument(document, tr_id, pointId);

    executer.executeRequest(obj);

    if (obj->RequestType == MESSAGE_TYPE::REGISTER)
    {
        return true;
    }
    return false;
}

void RequestAnalyser::parseLaunchRequest(std::string request, int tr_historic_id, int pointId)
{
    LOG_F(INFO, "-> Request received during a launch operation");
    nlohmann::json document = getJSONFromRequest(request);

    LOG_F(INFO, "tranform JSON into accurate object");

    Request *obj = getLaunchRequestsFromDocument(document, tr_historic_id, pointId);

    executer.executeHistoricSave(obj, tr_historic_id);
}

nlohmann::json RequestAnalyser::getJSONFromRequest(string request)
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

Request *RequestAnalyser::getSaveRequestsFromDocument(nlohmann::json &document, int tr_id, int pointId)
{
    string requestType = document["requestType"];
    LOG_F(INFO, "REQUEST FOUND : %s", requestType.c_str());
    MESSAGE_TYPE req = messagesTypeMap[requestType];
    switch (req)
    {
    case MESSAGE_TYPE::REGISTER:
        LOG_F(INFO, "-> REQUEST RECEIVED : REGISTER");
        return parseDroneDataRequest(tr_id, pointId, document);
        break;
    case MESSAGE_TYPE::END_TR_SAVE:
        LOG_F(INFO, "-> REQUEST RECEIVED : END_TR_SAVE");
        return parseEndTripSaveRequest(document);
        break;
    default:
        LOG_F(INFO, "-> REQUEST RECEIVED NOT IDENTIFIED");
        break;
    }
    return nullptr;
}

Request *RequestAnalyser::getLaunchRequestsFromDocument(nlohmann::json &document, int tr_historic_id, int pointId)
{
    string requestType = document["requestType"];
    LOG_F(INFO, "REQUEST FOUND : %s", requestType.c_str());
    MESSAGE_TYPE req = messagesTypeMap[requestType];
    switch (req)
    {
    case MESSAGE_TYPE::REGISTER:
        LOG_F(INFO, "-> REQUEST RECEIVED : REGISTER");
        return parseDroneDataRequest(tr_historic_id, pointId, document);
        break;
        LOG_F(INFO, "-> REQUEST RECEIVED NOT IDENTIFIED");
        break;
    }
    return nullptr;
}

Request *RequestAnalyser::getRequestFromDocument(nlohmann::json &document)
{
    string requestType = document["requestType"];
    LOG_F(INFO, "REQUEST FOUND : %s", requestType.c_str());
    MESSAGE_TYPE req = messagesTypeMap[requestType];
    switch (req)
    {
    case MESSAGE_TYPE::REQUEST:
        LOG_F(INFO, "-> REQUEST RECEIVED : REQUEST");
        return parseDataRequest(document);
        break;
    case MESSAGE_TYPE::TR_LAUNCH:
        LOG_F(INFO, "-> REQUEST RECEIVED : TR_LAUNCH");
        return parseTripLaunchRequest(document);
        break;
    case MESSAGE_TYPE::TR_SAVE:
        LOG_F(INFO, "-> REQUEST RECEIVED : TR_SAVE");
        return parseTripSaveRequest(document);
        break;
    case MESSAGE_TYPE::GET_PATH_LIST:
        LOG_F(INFO, "-> REQUEST RECEIVED : GET_PATH_LIST");
        return parsePathListRequest(document);
        break;
    case MESSAGE_TYPE::GET_ONE_PATH:
        LOG_F(INFO, "-> REQUEST RECEIVED : GET_ONE_PATH");
        return parseOnePathRequest(document);
        break;
    default:
        LOG_F(INFO, "-> REQUEST RECEIVED NOT IDENTIFIED");
        break;
    }
    return nullptr;
}

DataRequest *RequestAnalyser::parseDataRequest(nlohmann::json &obj)
{
    string valueType = obj["valueType"];
    int tr_id = obj["tr_id"].get<int>();
    int tr_position = obj["position_id"].get<int>();
    return new DataRequest(valueType, tr_id, tr_position);
}

DroneDataRegister *RequestAnalyser::parseDroneDataRequest(int tr_id, int pointId, nlohmann::json &obj)
{
    int altitude = obj["altitude"].get<int>();
    int latitude = obj["latitude"].get<int>();
    int longitude = obj["longitude"].get<int>();
    int rotation = obj["rotation"].get<int>();
    time_t date = obj["time"].get<time_t>();
    bool isCheckpoint = obj["isCheckpoint"].get<bool>();
    float temperature = obj["temperature"].get<float>();
    float pressure = obj["pressure"].get<float>();
    int batteryRemaining = obj["batteryRemaining"].get<int>();
    int image = obj["image"].get<ssize_t>();
    return new DroneDataRegister(tr_id, pointId, altitude, latitude, longitude, rotation, isCheckpoint, pressure, temperature, batteryRemaining, date, image);
}

TripSaveRequest *RequestAnalyser::parseTripSaveRequest(nlohmann::json &obj)
{
    return new TripSaveRequest();
}

endTripSaveRequest *RequestAnalyser::parseEndTripSaveRequest(nlohmann::json &obj)
{
    string tripName = obj["tripName"];
    return new endTripSaveRequest(tripName);
}

TripLaunchRequest *RequestAnalyser::parseTripLaunchRequest(nlohmann::json &obj)
{
    int tr_id = obj["tr_id"].get<int>();
    return new TripLaunchRequest(tr_id);
}

getOnePath *RequestAnalyser::parseOnePathRequest(nlohmann::json &obj)
{
    int tr_id = obj["tr_id"].get<int>();
    return new getOnePath(tr_id);
}

getPathList *RequestAnalyser::parsePathListRequest(nlohmann::json &obj)
{
    return new getPathList();
}