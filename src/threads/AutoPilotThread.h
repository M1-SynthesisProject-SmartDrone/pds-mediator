#if !defined(__AUTOPILOTTHREAD_H__)
#define __AUTOPILOTTHREAD_H__

#include <memory>

#include "Abstract_ThreadClass.h"
#include "../request_analyse/RequestAnalyser.h"
#include "../request_executer/RequestExecuter.h"
#include "postgresql/PostgresqlConnection.h"


class AutoPilotThread : public Abstract_ThreadClass {

    private:

    pqxx::stateless_cursor<pqxx::cursor_base::read_only, pqxx::cursor_base::owned> getPointsFromPG(int tr_id);

    unique_ptr<PostgresqlConnection> postgresConnection;

    unique_ptr<MongodbConnection> mongodbConnection;

    public:

    //RequestAnalyser analyser;
    RequestExecuter executer;



    AutoPilotThread(ConfigParams conf, std::shared_ptr<TCPSocket> inputSocket, std::shared_ptr<TCPSocket> outputSocket, PostgresqlConnection postgresConnection);

    ~AutoPilotThread();

    void run();

};

#endif // __AUTOPILOTTHREAD_H__
