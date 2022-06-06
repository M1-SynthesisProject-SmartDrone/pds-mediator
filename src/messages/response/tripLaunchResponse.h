#if !defined(__TRIPLAUNCHRESPONSE_H__)
#define __TRIPLAUNCHRESPONSE_H__

#include "Response.h"
#include "../messagetype.h"

struct TripLaunchResponse : Response {

    bool isDone;

    TripLaunchResponse(bool isDone){
        this->ResponseType = MESSAGE_TYPE::RESP_TR_LAUNCH;
        this->isDone = isDone;
    }
};

#endif // __TRIPLAUNCHRESPONSE_H__
