#include "RequestExecuter.h"
#include "../messages/messagetype.h"
#include <loguru/loguru.hpp>

RequestExecuter::RequestExecuter(){

}

RequestExecuter::~RequestExecuter(){
    
}

void RequestExecuter::executeRequest(Request* request){
    switch (request->RequestType)
    {
    case MESSAGE_TYPE::TR_SAVE :{
            TripSaveRequest* tripSaveRequest = static_cast<TripSaveRequest*>(request);
            executeTripSaveRequest(tripSaveRequest);
        }
        break;
    case MESSAGE_TYPE::TR_LAUNCH :{
            TripLaunchRequest* tripLaunchRequest = static_cast<TripLaunchRequest*>(request);
            executeTripLaunchRequest(tripLaunchRequest);
        }
        break;
    case MESSAGE_TYPE::REQUEST :{
            DataRequest* dataRequest = static_cast<DataRequest*>(request);
            executeDataRequest(dataRequest);
        }
        break;
    default:
        break;
    }
}

void RequestExecuter::executeDataRequest(DataRequest* dataRequest){
        LOG_F(INFO, "Requesting Data on the databases ... ");

}

void RequestExecuter::executeDroneDataRegister(DroneDataRegister* dronedataRegister){
    
}

void RequestExecuter::executeTripLaunchRequest(TripLaunchRequest* tripLaunchRequest){
        LOG_F(INFO, "Launch a registered trip ... ");

}

void RequestExecuter::executeTripSaveRequest(TripSaveRequest* TripSaveRequest){
    LOG_F(INFO, "Start saving a trip... ");



}

void RequestExecuter::executeEndTripSaveRequest(endTripSaveRequest* endTripSaveRequest){

}
