#if !defined(__AUTOPILOTTHREAD_H__)
#define __AUTOPILOTTHREAD_H__

#include <memory>

#include "Abstract_ThreadClass.h"
#include "../request_analyse/RequestAnalyser.h"
#include "../request_executer/RequestExecuter.h"
#include "postgresql/PostgresqlConnection.h"
#include "../config/config_parser.h"
#include "../messages/response/RespReqTrippoints.h"
#include "../messages/response/dronePosition.h"



class AutoPilotThread : public Abstract_ThreadClass {

    private:


    nlohmann::json getJSONFromRequest(string request);

    void sendAllTrip(int tr_id);

    nlohmann::json convertRespReqTripPoints(respTripPoints* response);
    
    nlohmann::json convertDronePosResponse(dronePosition* response);

    public:

    //RequestAnalyser analyser;
    //RequestExecuter executer;

    int tripid;

    pqxx::stream_from getPointsFromPG(int tr_id);


    ConfigParams config;
    
    std::shared_ptr<TCPSocket> dataOutput;

    std::unique_ptr<PostgresqlConnection> postgresConnection;

    std::unique_ptr<MongodbConnection> mongodbConnection;

    AutoPilotThread(ConfigParams conf, std::shared_ptr<TCPSocket> outputSocket,  int tr_id);

    ~AutoPilotThread();

    void run();

};

#endif // __AUTOPILOTTHREAD_H__
