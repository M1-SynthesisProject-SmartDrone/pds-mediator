#ifndef __TRIPLAUNCHREQUEST_H__
#define __TRIPLAUNCHREQUEST_H__

#include "Request.h"
#include "../messagetype.h"

using namespace std;

struct TripLaunchRequest: Request
{
    int tr_id;

    TripLaunchRequest(int tr_id){
        this->RequestType = MESSAGE_TYPE::TR_LAUNCH;
        this-> tr_id = tr_id;
    }
    
};

#endif // __TRIPLAUNCHREQUEST_H__