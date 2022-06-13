#if !defined(__RESPENDTRIPLAUNCH_H__)
#define __RESPENDTRIPLAUNCH_H__
#include "Response.h"

struct respEndTripLaunch : Response {

    respEndTripLaunch(){
        this->ResponseType = MESSAGE_TYPE::RESP_END_TR_LAUNCH;
    }
};


#endif // __RESPENDTRIPLAUNCH_H__
