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

using namespace std;
using namespace nlohmann;

RequestAnalyser::RequestAnalyser(){
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

    LOG_F(INFO,"tranform JSON into accurate object");
    Request* obj = getRequestFromDocument(document );

    executer.executeRequest(obj);
    
}


nlohmann::json RequestAnalyser::getJSONFromRequest(string request){
    LOG_F(INFO,"Parsing received request");
    nlohmann::json json = nlohmann::json::parse(request);
    if (!json["requestType"].is_string())
    {
        throw invalid_argument("No requestType object found");
    }
    LOG_F(INFO,"Parsing received request done !");
    return json;
}

Request* RequestAnalyser::getRequestFromDocument(nlohmann::json& document){
    string requestType = document["requestType"];
    LOG_F(INFO,"REQUEST FOUND : %s", requestType.c_str());
    MESSAGE_TYPE req = messagesTypeMap[requestType] ;
    switch ( req )
    {
    case MESSAGE_TYPE::REQUEST :
        LOG_F(INFO, "-> REQUEST RECEIVED : REQUEST");
        return parseDataRequest(document);
        break;
    case MESSAGE_TYPE::TR_LAUNCH :
        LOG_F(INFO, "-> REQUEST RECEIVED : TR_LAUNCH");
        return parseTripLaunchRequest(document);
        break;
    case MESSAGE_TYPE::TR_SAVE : 
        LOG_F(INFO, "-> REQUEST RECEIVED : TR_SAVE");
        return parseTripSaveRequest(document);
        break;
    // case MESSAGE_TYPE::END_TR_SAVE : 
    //     LOG_F(INFO, "-> REQUEST RECEIVED : END_TR_SAVE");
    //     return parseEndTripSaveRequest(document);
    //     break;
    default:
        LOG_F(INFO, "-> REQUEST RECEIVED NOT IDENTIFIED");
        break;
    }
    return nullptr;
}



DataRequest* RequestAnalyser::parseDataRequest(nlohmann::json& obj){
    string valueType = obj["valueType"];
    int tr_id = obj["tr_id"].get<int>();
    int tr_position = obj["position_id"].get<int>();
    return new DataRequest(valueType, tr_id, tr_position);
}

DroneDataRegister* RequestAnalyser::parseDroneDataRequest(int tr_id, nlohmann::json& obj){
    int latitude = obj["latitude"].get<int>();
    int longitude = obj["longitude"].get<int>();
    int rotation = obj["rotation"].get<int>();
    time_t date = obj["time"].get<time_t>();
    string imageName = obj["imageName"];
    return new DroneDataRegister(tr_id, latitude, longitude, rotation, date, imageName);
}

TripSaveRequest* RequestAnalyser::parseTripSaveRequest(nlohmann::json& obj){
    return new TripSaveRequest();

}

endTripSaveRequest* RequestAnalyser::parseEndTripSaveRequest(nlohmann::json& obj){
    string tripName = obj["tripName"];
    return new endTripSaveRequest(tripName);

}

TripLaunchRequest* RequestAnalyser::parseTripLaunchRequest(nlohmann::json& obj){
    return nullptr;

}