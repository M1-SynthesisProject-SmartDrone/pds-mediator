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
    case MESSAGE_TYPE::RESP_REGISTER:
        {
            LOG_F(INFO,"Converting RespRegister request to JSON ...");
            DataRegisterResponse* dataRegisterResponse = static_cast<DataRegisterResponse*>(response);
            return convertRespRegister(dataRegisterResponse);
        }  
    break;
    case MESSAGE_TYPE::RESP_TR_LAUNCH:
        {
            LOG_F(INFO,"Converting TripLaunchResponse request to JSON ...");
            TripLaunchResponse* tripLaunchResponse = static_cast<TripLaunchResponse*>(response);
            return convertRespTripLaunch(tripLaunchResponse);
        }  
    break;
    case MESSAGE_TYPE::ERROR_NOTIFICATION_RECEIVED :
        {
            LOG_F(INFO,"Converting TripLaunchResponse request to JSON ...");
            TripLaunchResponse* tripLaunchResponse = static_cast<TripLaunchResponse*>(response);
            return convertRespTripLaunch(tripLaunchResponse);
        }  
     case MESSAGE_TYPE::RESP_ONE_PATH :
        {
            LOG_F(INFO,"Converting respOnePath request to JSON ...");
            onePathResponse* onepathresp = static_cast<onePathResponse*>(response);
            return convertOnePathResponse(onepathresp);
        }  
    break;
    default:
        break;
    }
}

nlohmann::json JSON_Converter::convertRespTripLaunch(respErrorNotif* response){
    nlohmann::json document;
    document["responseType"] = REQUESTTYPES[response->ResponseType];
    return document;
}

nlohmann::json JSON_Converter::convertOnePathResponse(onePathResponse* response){
    nlohmann::json document;
    document["responseType"] = REQUESTTYPES[response->ResponseType];
    document["name"] = response->name;
    document["id"] = response->id;
    document["nbPoints"] = response->nbPoints;
    document["nbCheckpoints"] = response->nbCheckpoints;
    document["date"] = response->date;
    document["latitude"] = response->latitude;
    document["longitude"] = response->longitude;
    document["altitude"] = response->altitude;

    return document;
}


nlohmann::json JSON_Converter::convertRespTripLaunch(TripLaunchResponse* response){
    nlohmann::json document;   
    document["responseType"] = REQUESTTYPES[response->ResponseType];
    document["isDone"] = response->isDone;
    return document;
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

nlohmann::json JSON_Converter::convertRespRegister(DataRegisterResponse* response){
    nlohmann::json document;   
    document["responseType"] = REQUESTTYPES[response->ResponseType] ;
    document["isDone"] = response->isDone;
    return document;
}