#if !defined(__REQUESTEXECUTER_H__)
#define __REQUESTEXECUTER_H__

#include "../messages/request/Request.h"
#include "../messages/request/Request.h"
#include "../messages/request/DataRequest.h"
#include "../messages/request/droneData.h"
#include "../messages/request/endTripSaveRequest.h"
#include "../messages/request/TripLaunchRequest.h"
#include "../messages/request/TripSaveRequest.h"

#include "../messages/messagetype.h"

class RequestExecuter {

private:

public:

RequestExecuter();

~RequestExecuter();

void executeRequest(Request* request);

void executeDataRequest(DataRequest* dataRequest);

void executeDroneDataRegister(DroneDataRegister* dronedataRegister);

void executeTripLaunchRequest(TripLaunchRequest* tripLaunchRequest);

void executeTripSaveRequest(TripSaveRequest* TripSaveRequest);

void executeEndTripSaveRequest(endTripSaveRequest* endTripSaveRequest);

};


#endif // __REQUESTEXECUTER_H__
