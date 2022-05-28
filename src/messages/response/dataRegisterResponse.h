#if !defined(__DATAREGISTERRESPONSE_H__)
#define __DATAREGISTERRESPONSE_H__

#include "Response.h"

struct DataRegisterResponse : Response {
    bool isDone;

    DataRegisterResponse(bool isDone){
        this->ResponseType = MESSAGE_TYPE::RESP_REGISTER;
        this->isDone = isDone;
    }
};


#endif // __DATAREGISTERRESPONSE_H__
