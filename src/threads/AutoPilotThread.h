#if !defined(__AUTOPILOTTHREAD_H__)
#define __AUTOPILOTTHREAD_H__

#include <memory>

#include "Abstract_ThreadClass.h"
#include "../request_analyse/RequestAnalyser.h"
#include "../request_executer/RequestExecuter.h"
#include "postgresql/PostgresqlConnection.h"
#include "../config/config_parser.h"
#include "../messages/response/RespReqTrippoints.h"




class AutoPilotThread : public Abstract_ThreadClass {

    private:

    pqxx::stream_from getPointsFromPG(int tr_id);

    nlohmann::json getJSONFromRequest(string request);

    void sendAllTrip(pqxx::stream_from& stream);

    nlohmann::json convertRespReqTripPoints(respTripPoints* response);
    

    public:

    //RequestAnalyser analyser;
    //RequestExecuter executer;

    int tripid;

    ConfigParams config;
    
    std::shared_ptr<TCPSocket> dataOutput;

    unique_ptr<PostgresqlConnection> postgresConnection;

    unique_ptr<MongodbConnection> mongodbConnection;

    AutoPilotThread(ConfigParams conf, std::shared_ptr<TCPSocket> inputSocket, std::shared_ptr<TCPSocket> outputSocket,  int tr_id);

    ~AutoPilotThread();

    void run();

};

#endif // __AUTOPILOTTHREAD_H__
