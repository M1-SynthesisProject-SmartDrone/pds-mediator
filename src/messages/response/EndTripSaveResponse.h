#if !defined(__ENDTRIPSAVERESPONSE_H__)
#define __ENDTRIPSAVERESPONSE_H__

#include "Response.h"

struct EndTripSaveResponse : Response {

    bool isDone;

    EndTripSaveResponse(bool isDone){
        this->ResponseType = MESSAGE_TYPE::RESP_END_TR_SAVE;
        this->isDone = isDone;
    }



};


#endif // __ENDTRIPSAVERESPONSE_H__