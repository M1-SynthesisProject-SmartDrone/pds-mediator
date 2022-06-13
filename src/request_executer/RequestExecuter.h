#if !defined(__REQUESTEXECUTER_H__)
#define __REQUESTEXECUTER_H__

#include "../messages/request/Request.h"
#include "../messages/request/Request.h"
#include "../messages/request/DataRequest.h"
#include "../messages/request/droneData.h"
#include "../messages/request/endTripSaveRequest.h"
#include "../messages/request/TripLaunchRequest.h"
#include "../messages/request/TripSaveRequest.h"
#include "../messages/request/getPathList.h"
#include "../messages/request/getOnePath.h"
#include "../postgresql/PostgresqlConnection.h"
#include "../mongodb/MongodbConnection.h"
#include "../network/TCPSocket.h"
#include "../converter/JSON_Converter.h"
#include "../messages/messagetype.h"
#include <fstream>
#include "../messages/response/tripLaunchResponse.h"
#include "threads/bridges/ImageSaver_MessageHolder.h"


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

    void registerNewPositionTrip(int tripId, int pointId, DroneDataRegister* data, bool isHistoric = false, int idLaunch = -1);
    

    void registerImage(int tripId, int positionId, std::vector<uint8_t> image, int idLaunch = -1);

public:
    ConfigParams config;

    JSON_Converter converter;
    
    std::shared_ptr<TCPSocket> dataInput;

    std::shared_ptr<TCPSocket> dataOutput;

    std::shared_ptr<ImageSaver_MessageHolder> imageSaverHolder;

    RequestExecuter(ConfigParams conf, 
        std::shared_ptr<TCPSocket> inputSocket, 
        std::shared_ptr<TCPSocket> outputSocket,
        std::shared_ptr<ImageSaver_MessageHolder> imageSaverHolder
    );

    ~RequestExecuter();

    void executeRequest(Request *request);

    void executeHistoricSave(Request* request, int idLaunch);

    void executeDataRequest(DataRequest *dataRequest);

    void executeDroneDataRegister(DroneDataRegister *dronedataRegister);

    void executeTripLaunchRequest(TripLaunchRequest *tripLaunchRequest);

    void executeTripSaveRequest(TripSaveRequest *TripSaveRequest);

    void executeEndTripSaveRequest(endTripSaveRequest *endTripSaveRequest);

    void executeGetPathList(getPathList* pathListRequest);

    void executeGetOnePath(getOnePath* onePathRequest); 
};

#endif // __REQUESTEXECUTER_H__
