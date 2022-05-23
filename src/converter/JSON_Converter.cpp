#include "JSON_Converter.h"
#include "../messages/messagetype.h"

JSON_Converter::JSON_Converter(){

}

JSON_Converter::~JSON_Converter(){

}

Request* JSON_Converter::convertReceivedRequest(string request){
    return nullptr;
}

nlohmann::json JSON_Converter::convertToSendRequest(Response* response){
    switch (response->ResponseType)
    {
    case MESSAGE_TYPE::RESP_TR_SAVE :
        {
            LOG_F(INFO,"Converting startTripSaveResponse request to JSON ...");
            StartTripSaveResponse* startTripSaveResponse = static_cast<StartTripSaveResponse*>(response);
            return convertStartTripSaveResponse(startTripSaveResponse);
        }
        break;
    case MESSAGE_TYPE::RESP_END_TR_SAVE:
        {
            LOG_F(INFO,"Converting EndTripSaveResponse request to JSON ...");
            EndTripSaveResponse* endTripSaveResponse = static_cast<EndTripSaveResponse*>(response);
            return convertEndTripSaveResponse(endTripSaveResponse);
        }
    break;
    default:
        break;
    }
}


nlohmann::json JSON_Converter::convertStartTripSaveResponse(StartTripSaveResponse* response){
    nlohmann::json document;   
    document["responseType"] = REQUESTTYPES[response->ResponseType];
    document["isLaunched"] = response->isLaunched;
    return document;
}

nlohmann::json JSON_Converter::convertEndTripSaveResponse(EndTripSaveResponse* response){
    nlohmann::json document;   
    document["responseType"] = REQUESTTYPES[response->ResponseType] ;
    document["isDone"] = response->isDone;
    return document;
}