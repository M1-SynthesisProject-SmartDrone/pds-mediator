#if !defined(__NEXTDRONEPOSITION_H__)
#define __NEXTDRONEPOSITION_H__

#include "Request.h"
#include "../messagetype.h"

struct nextDronePosition : Request {
    int id_pos;
    int filesize;

    nextDronePosition(int id_pos, int filesize){
        this->RequestType = MESSAGE_TYPE::NEXTDRONEPOSITION;
        this->filesize = filesize;
        this->id_pos = id_pos;
    }
};


#endif // __NEXTDRONEPOSITION_H__
