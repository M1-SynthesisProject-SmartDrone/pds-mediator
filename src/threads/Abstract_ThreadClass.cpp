/**
 *  @author Sylvain Colomer
 *  @date 18/04/19.
 */


#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

#include <sys/time.h>
#include "threads/Abstract_ThreadClass.h"

//%%%%%%%%%%%%%%%%%%%%%%%%% begin/end phase function %%%%%%%%%%%%%%%%%%%%%%%%%%%%

Abstract_ThreadClass::Abstract_ThreadClass(int task_period, int task_deadline)
{
    task_period=task_period;
    task_deadline=task_deadline;
    cout << "task_period = " << task_period <<endl;
}


Abstract_ThreadClass::~Abstract_ThreadClass()
{

    currentState = LifeCoreState::QUIT;

    if(runFlag){
        stop();
    }
}

//%%%%%%%%%%%%%%%%%%%%%%%%% run function %%%%%%%%%%%%%%%%%%%%%%%%%%%%

void Abstract_ThreadClass::run()
{

    long long currentThreadDelay;

    gettimeofday(&begin, 0);
    gettimeofday(&front_checkpoint, 0);

    currentState = LifeCoreState::RUN;

    while(isRunFlag())
    {
        usleep(task_period);
        
        cout<<"abstract class thread : run() "<<endl;

        gettimeofday(&end_checkpoint, 0);
        currentThreadDelay=(end_checkpoint.tv_sec-front_checkpoint.tv_sec) * 1000000L + (end_checkpoint.tv_usec-front_checkpoint.tv_usec);

        if (currentThreadDelay > task_period )
        {
            gettimeofday(&front_checkpoint, 0);

            if (currentThreadDelay > task_period + task_deadline)
            {
                currentState = LifeCoreState::DEADLINE_EXCEEDED;
            }
            else 
            
                currentState = LifeCoreState::RUN;
           //}
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
    principalThread= std::thread(&Abstract_ThreadClass::run, this);
}


void Abstract_ThreadClass::stop()
{
    currentState = LifeCoreState::STOP;
    setRunFlag(false);
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
    runFlag = runFlag;
    runFlag_mutex.unlock();

}

LifeCoreState Abstract_ThreadClass::getCurrentState() const 
{
    return currentState;
}


