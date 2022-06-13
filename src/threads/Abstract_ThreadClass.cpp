/**
 *  @author Sylvain Colomer
 *  @date 18/04/19.
 */


#include <iostream>
#include <string>
#include <cstdlib>
#include <sys/time.h>

#include <loguru/loguru.hpp>

#include "Abstract_ThreadClass.h"

using namespace std;

//%%%%%%%%%%%%%%%%%%%%%%%%% begin/end phase function %%%%%%%%%%%%%%%%%%%%%%%%%%%%

// lastLoopTime == task_period in order to not wait on the first iteration of the run
Abstract_ThreadClass::Abstract_ThreadClass(string id, int task_period, int task_deadline)
    : task_deadline(task_deadline), task_period(task_period), id(id), lastLoopTime(task_period)
{}

Abstract_ThreadClass::~Abstract_ThreadClass()
{
    currentState = LifeCoreState::QUIT;

    if (runFlag)
    {
        stop();
    }
}

//%%%%%%%%%%%%%%%%%%%%%%%%% run function %%%%%%%%%%%%%%%%%%%%%%%%%%%%

// ==== Modifs ====
void Abstract_ThreadClass::initRun()
{
    loguru::set_thread_name(id.c_str());
    gettimeofday(&begin, NULL);
    LOG_F(INFO, "Start thread %s", id.c_str());
}

void Abstract_ThreadClass::onStartLoop()
{
    // We don't want to start before the next period
    if (lastLoopTime < task_period)
    {
        usleep((task_period - lastLoopTime) * 1000);
    }

    gettimeofday(&front_checkpoint, NULL);
}

void Abstract_ThreadClass::onEndLoop()
{
    gettimeofday(&end_checkpoint, NULL);

    lastLoopTime = (end_checkpoint.tv_sec - front_checkpoint.tv_sec) * 1000000L + (end_checkpoint.tv_usec - front_checkpoint.tv_usec);
    lastLoopTime /= 1000;
    if (lastLoopTime > task_deadline)
    {
        LOG_F(ERROR, "Task deadline exedeed (expected %dms but got %ldms)", task_deadline, lastLoopTime);
    }
}
// ==== End Modifs ====

void Abstract_ThreadClass::run()
{

    long long currentThreadDelay;

    gettimeofday(&begin, 0);
    gettimeofday(&front_checkpoint, 0);

    currentState = LifeCoreState::RUN;

    while (isRunFlag())
    {
        usleep(task_period);

        cout << "abstract class thread : run() " << endl;

        gettimeofday(&end_checkpoint, 0);
        currentThreadDelay = (end_checkpoint.tv_sec - front_checkpoint.tv_sec) * 1000000L + (end_checkpoint.tv_usec - front_checkpoint.tv_usec);

        if (currentThreadDelay > task_period)
        {
            gettimeofday(&front_checkpoint, 0);

            if (currentThreadDelay > task_period + task_deadline)
            {
                currentState = LifeCoreState::DEADLINE_EXCEEDED;
            }
            else
            {

                currentState = LifeCoreState::RUN;
            }
        }
    }
}

//%%%%%%%%%%%%%%%%%%%%%%%%% control function %%%%%%%%%%%%%%%%%%%%%%%%%%%%

void Abstract_ThreadClass::init()
{
    currentState = LifeCoreState::INIT;
    currentState = LifeCoreState::READY;
}

void Abstract_ThreadClass::start()
{
    setRunFlag(true);
    currentState = LifeCoreState::RUN;
    principalThread = std::thread(&Abstract_ThreadClass::run, this);
}


void Abstract_ThreadClass::stop()
{
    currentState = LifeCoreState::STOP;
    setRunFlag(false);
    LOG_F(INFO, "Wait for thread %s to end", id.c_str());
    principalThread.join();
}

void Abstract_ThreadClass::lazyStop()
{
    currentState = LifeCoreState::STOP;
    setRunFlag(false);
}

void Abstract_ThreadClass::play()
{
    currentState = LifeCoreState::RUN;
    setRunFlag(true);
}

void Abstract_ThreadClass::pause()
{

}

void Abstract_ThreadClass::join()
{
    principalThread.join();
}

//%%%%%%%%%%%%%%%%%%%%%%%%% getters and setters %%%%%%%%%%%%%%%%%%%%%%%%%%%%

bool Abstract_ThreadClass::isRunFlag() const
{
    return runFlag;
}


void Abstract_ThreadClass::setRunFlag(bool runFlag)
{
    runFlag_mutex.lock();
    this->runFlag = runFlag;
    runFlag_mutex.unlock();
}

LifeCoreState Abstract_ThreadClass::getCurrentState() const
{
    return currentState;
}