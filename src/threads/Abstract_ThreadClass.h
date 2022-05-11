/**
 * @author Sylvain Colomer
 * @date 17/08/18.
 */

#ifndef ABSTRACT_THREADCLASS_H_
#define ABSTRACT_THREADCLASS_H_

#include <iostream>
#include <string>
#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <thread>
#include <mutex>
#include <functional>
#include <memory>
#include <algorithm>

/**
 * A thread have multiple state during is life
 *  TO_INIT -> Not initialised thread
 *  INIT -> Thread in initialisation
 *  READY -> Thread ready to be start
 *  RUN -> Thread which work
 *  STOP -> Thread stopped
 *  QUIT -> Thread in quit process
 *  PROBLEM -> Blocked thread in reason of a problem
 *  DEADLINE_EXCEEDED -> thread
 *
 */
enum LifeCoreState {
    TO_INIT,
    INIT,
    READY,
    RUN,
    STOP,
    QUIT,
    PROBLEM,
    DEADLINE_EXCEEDED
};

/**
 * Abstract class which convey to implement a thread brick
 * Use a real time context
 */
class Abstract_ThreadClass {
    

public :
    // REAL TIME VAR
    /**
     * timeval that save the begin of the running method
     */
    struct timeval begin;

    /**
     * timeval use to control the period of the real time thread
     */
    struct timeval front_checkpoint;

    /**
     * timeval use to control the period of the real time thread
     */
    struct timeval end_checkpoint;

    /**
     * Represent the period between all task in ms
     */
    int task_period = 1000;
    
    /**
     * Var use to define when a task is overcome is execution delay
     */
    int task_deadline = 1000; //ms

    // RUN VAR
    /**
     * Mutex of the run flag
     */
    std::mutex runFlag_mutex;
    /**
     * Run flag : convey to loop the system
     */
    bool runFlag = true;

    // MAIN THREAD VAR
    /**
     * The principal thread of the system
     */
    std::thread principalThread;

    /**
     * Var use to share the state of the system
     */
    LifeCoreState currentState=LifeCoreState::TO_INIT;;



    /**
     * Default constructor : take in parameters the task period time and the task deadline time
     * @param task_period period between all task execution
     * @param task_deadline alert limit use to send an alert message when the task are too long (see the notion of real time context)
     */
    Abstract_ThreadClass(int task_period, int task_deadline);

    /**
     * Default destructor
     */
    virtual ~Abstract_ThreadClass();

    /**
     * Method which convey to initialise the thread
     * Don't overwrite it without knowledge of the method goal
     */
    void init();

    /**
     * Start method : use it to first start the thread (only one time)
     * It create the thread associated to the run method
     */
    void start();

    /**
     * Convey to stop the thread.
     */
    void stop();

    /**
     * Stop function : convey to stop the thread without join()
     * Warning : it usage is not recommended if the user don't understand it interest. (usually to exit the main core)
     */
    void lazyStop();

    /**
     * Play method : convey to restart the thread before a first start and after a pause
     */
    void play();

    /**
     * Pause method : convey to pause the thread without quit it
     */
    void pause();

    /**
     * Method wich allow to join the thread
     * Warning : don't double join a thread
     */
    void join();

    /**
     * Method main loop. Implement a soft real time context
     * Overwrite it to create your real time thread
     */
    virtual void run() = 0;

    // GETTER AND SETTERS

    /**
     * Only a setter
     * @return
     */
    bool isRunFlag() const;

    /**
     * Setter of the run mutex : need to unlock the mutex
     * @param runFlag
     */
    void setRunFlag(bool runFlag);

    /**
     * Only a getter
     * @return
     */
    LifeCoreState getCurrentState() const;

};

#endif
