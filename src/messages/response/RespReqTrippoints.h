#if !defined(__RESPTRIPPOINTS_H__)
#define __RESPTRIPPOINTS_H__


#include "Response.h"
#include "../messagetype.h"

struct respTripPoints : Response {
    int fileSize;

    respTripPoints(int fileSize){
        this->ResponseType = MESSAGE_TYPE::RESP_REQ_TRIP_POINTS;
        this->fileSize = fileSize;
    }
};

#endif // __RESPTRIPPOINTS_H__
