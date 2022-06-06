#ifndef __REQUEST_ANALYSER_H__
#define __REQUEST_ANALYSER_H__

#include <nlohmann/json.hpp>

#include "config/config_parser.h"
#include "../messages/request/Request.h"
#include "../messages/request/DataRequest.h"
#include "../messages/request/droneData.h"
#include "../messages/request/endTripSaveRequest.h"
#include "../messages/request/TripLaunchRequest.h"
#include "../messages/request/TripSaveRequest.h"
#include "../messages/request/getOnePath.h"
#include "../messages/request/getPathList.h"

#include "../network/TCPSocket.h"

#include "../messages/messagetype.h"
#include "../request_executer/RequestExecuter.h"

class RequestAnalyser
{

public:
    RequestAnalyser();

    RequestAnalyser(ConfigParams conf, std::shared_ptr<TCPSocket> inputSocket, std::shared_ptr<TCPSocket> outputSocket);

    ~RequestAnalyser();

    void parseRequest(std::string request);

    bool parseSaveRequest(std::string request, int tr_id, int pointId);

    void parseLaunchRequest(std::string request, int tr_historic_id, int pointId);

    bool parsePositionRequest(string request);

    nlohmann::json getJSONFromRequest(std::string request);

    DroneDataRegister *parseDroneDataRequest(int tr_id, int pointId, nlohmann::json &obj);

    std::unordered_map<std::string, MESSAGE_TYPE> messagesTypeMap{
        {"REQUEST", REQUEST},
        {"TR_LAUNCH", TR_LAUNCH},
        {"END_TR_LAUNCH", END_TR_LAUNCH},
        {"TR_SAVE", TR_SAVE},
        {"END_TR_SAVE", END_TR_SAVE},
        {"REGISTER", REGISTER},
        {"NEXTDRONEPOSITION", NEXTDRONEPOSITION},
        {"REQ_TR_POINTS", REQ_TR_POINTS},
        {"GET_PATH_LIST", GET_PATH_LIST},
        {"GET_ONE_PATH", GET_ONE_PATH},
        {"TR_FILE", TR_FILE},
        {"WAIT_TR_FILE", WAIT_TR_FILE},
        {"TR_ERROR", TR_ERROR},
        {"END_TR_ERROR", END_TR_ERROR},
        {"RESP_REQUEST", RESP_REQUEST},
        {"RESP_TR_LAUNCH", RESP_TR_LAUNCH},
        {"RESP_END_TR_LAUNCH", RESP_END_TR_LAUNCH},
        {"RESP_TR_SAVE", RESP_TR_SAVE},
        {"RESP_END_TR_SAVE", RESP_END_TR_SAVE},
        {"RESP_REGISTER", RESP_REGISTER},
        {"RESP_REQ_TRIP_POINTS", RESP_REQ_TRIP_POINTS},
        {"RESP_PATH_LIST", RESP_PATH_LIST},
        {"RESP_ONE_PATH", RESP_ONE_PATH},
        {"RESP_TR_FILE", RESP_TR_FILE},
        {"RESP_DRONEPOSITION", RESP_DRONEPOSITION},
        {"ERROR_NOTIFICATION_RECEIVED", ERROR_NOTIFICATION_RECEIVED},
        {"UNKNOWN", UNKNOWN}};

private:
    ConfigParams config;

    RequestExecuter executer;

    // register the reference to the tripSendingThread

    Request *getRequestFromDocument(nlohmann::json &document);

    Request *getSaveRequestsFromDocument(nlohmann::json &document, int tr_id, int pointId);

    Request *getLaunchRequestsFromDocument(nlohmann::json &document, int tr_historic_id, int pointId);

    DataRequest *parseDataRequest(nlohmann::json &obj);

    TripSaveRequest *parseTripSaveRequest(nlohmann::json &obj);

    endTripSaveRequest *parseEndTripSaveRequest(nlohmann::json &obj);

    TripLaunchRequest *parseTripLaunchRequest(nlohmann::json &obj);

    getOnePath *parseOnePathRequest(nlohmann::json &obj);

    getPathList *parsePathListRequest(nlohmann::json &obj);

    // const std::unordered_map<
    //     std::string,
    //     // This is a bit long, but it is better not to throw aliases in header files
    //     // And yes, lambdas are a bit ugly in c++, but safier than raw function pointers (and std::bind)
    //     std::function<Request*(rapidjson::GenericObject<false, rapidjson::Value>&)>
    //     > CONVERTER_FROM_STR {
    //     {getStringFromEnum(MESSAGE_TYPE::REQUEST) , [this](auto content) {return parseDataRequest(content);}},
    //     {getStringFromEnum(MESSAGE_TYPE::REGISTER) , [this](auto content) {return parseDroneDataRequest(content);}},
    //     {getStringFromEnum(MESSAGE_TYPE::TR_SAVE) , [this](auto content) {return parseTripSaveRequest(content);}},
    //     {getStringFromEnum(MESSAGE_TYPE::END_TR_LAUNCH) , [this](auto content) {return parseEndTripSaveRequest(content);}},
    //     {getStringFromEnum(MESSAGE_TYPE::TR_LAUNCH) , [this](auto content) {return parseTripLaunchRequest(content);}}
    // };
};

#endif // __REQUEST_ANALYSER_H__