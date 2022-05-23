#if !defined(__STARTTRIPSAVEREQUEST_H__)
#define __STARTTRIPSAVEREQUEST_H__

#include "Response.h"
#include "../messagetype.h"

struct StartTripSaveResponse : Response {
    bool isLaunched;

    StartTripSaveResponse(bool isLaunched){
        this->ResponseType = MESSAGE_TYPE::RESP_TR_SAVE;
        this->isLaunched = isLaunched;
    }
};

#endif // __STARTTRIPSAVEREQUEST_H__
