#include "AutoPilotThread.h"
#include <loguru/loguru.hpp>


AutoPilotThread::AutoPilotThread(ConfigParams conf, std::shared_ptr<TCPSocket> inputSocket, std::shared_ptr<TCPSocket> outputSocket) : Abstract_ThreadClass(1000, 200), executer(conf, inputSocket, outputSocket) {

}

AutoPilotThread::~AutoPilotThread(){

}

pqxx::stateless_cursor<pqxx::cursor_base::read_only, pqxx::cursor_base::owned> getPointsFromPG(int tr_id){
    auto transaction3 = postgresConnection->createTransaction();
    string query = "SELECT * FROM test;";
    pqxx::stateless_cursor<pqxx::cursor_base::read_only, pqxx::cursor_base::owned>
    cursor(*(transaction3.get()), query, "myCursor", false);
}


void AutoPilotThread::run()
{
    loguru::set_thread_name("Position Sender");
    // wait messages
    LOG_F(INFO, "Start waiting for position request");
    while (isRunFlag())
    {
        usleep(task_period);
        try
        {
            //"SELECT latitude, longitude, rotation, height, FROM tr_basic_points WHERE idTrajet = $1 AND isCheckpoint = true"
        }
        catch (const std::exception &e)
        {
            LOG_F(ERROR, e.what());
        }
    }
}