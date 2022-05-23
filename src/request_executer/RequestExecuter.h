#if !defined(__REQUESTEXECUTER_H__)
#define __REQUESTEXECUTER_H__

#include "../messages/request/Request.h"
#include "../messages/request/Request.h"
#include "../messages/request/DataRequest.h"
#include "../messages/request/droneData.h"
#include "../messages/request/endTripSaveRequest.h"
#include "../messages/request/TripLaunchRequest.h"
#include "../messages/request/TripSaveRequest.h"
#include "../postgresql/PostgresqlConnection.h"
#include "../mongodb/MongodbConnection.h"
#include "../network/UDPSocket.h"
#include "../converter/JSON_Converter.h"

#include "../messages/messagetype.h"

class RequestExecuter
{

private:
    unique_ptr<PostgresqlConnection> postgresConnection;

    unique_ptr<MongodbConnection> mongodbConnection;

    int getNewTripId();

    void registerNewTripEntry(int tripId);

    void sendRespTripSave(bool isLaunched);

    void registerNewPositionBasicTrip(int tripId, int pointId, DroneDataRegister* data);

    void registerImage(int tripId, int positionId, std::string image);

public:
    ConfigParams config;

    JSON_Converter converter;
    
    std::shared_ptr<UDPSocket> dataInput;

    std::shared_ptr<UDPSocket> dataOutput;

    RequestExecuter(ConfigParams conf, std::shared_ptr<UDPSocket> inputSocket, std::shared_ptr<UDPSocket> outputSocket);

    ~RequestExecuter();

    void executeRequest(Request *request);

    void executeDataRequest(DataRequest *dataRequest);

    void executeDroneDataRegister(DroneDataRegister *dronedataRegister);

    void executeTripLaunchRequest(TripLaunchRequest *tripLaunchRequest);

    void executeTripSaveRequest(TripSaveRequest *TripSaveRequest);

    void executeEndTripSaveRequest(endTripSaveRequest *endTripSaveRequest);
};

#endif // __REQUESTEXECUTER_H__
