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
#include "../network/TCPSocket.h"
#include "../converter/JSON_Converter.h"
#include "../messages/messagetype.h"
#include <fstream>

class RequestExecuter
{

private:
    unique_ptr<PostgresqlConnection> postgresConnection;

    unique_ptr<MongodbConnection> mongodbConnection;

    int getNewTripId();

    int getNewHistoricTripId();

    void registerNewTripEntry(int tripId);

    void registerNewHistoricTripEntry(int tripId, int basicTripId);

    void sendRespTripSave(bool isLaunched);

    void registerNewPositionBasicTrip(int tripId, int pointId, DroneDataRegister* data);
    

    void registerImage(int tripId, int positionId, std::vector<uint8_t> image);

public:
    ConfigParams config;

    JSON_Converter converter;
    
    std::shared_ptr<TCPSocket> dataInput;

    std::shared_ptr<TCPSocket> dataOutput;

    RequestExecuter(ConfigParams conf, std::shared_ptr<TCPSocket> inputSocket, std::shared_ptr<TCPSocket> outputSocket);

    ~RequestExecuter();

    void executeRequest(Request *request);

    void executeHistoricSave(Request* request, int idLaunch);

    void executeDataRequest(DataRequest *dataRequest);

    void executeDroneDataRegister(DroneDataRegister *dronedataRegister);

    void executeTripLaunchRequest(TripLaunchRequest *tripLaunchRequest);

    void executeTripSaveRequest(TripSaveRequest *TripSaveRequest);

    void executeEndTripSaveRequest(endTripSaveRequest *endTripSaveRequest);
};

#endif // __REQUESTEXECUTER_H__
