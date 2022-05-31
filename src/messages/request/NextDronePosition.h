#if !defined(__NEXTDRONEPOSITION_H__)
#define __NEXTDRONEPOSITION_H__

#include "Request.h"
#include "../messagetype.h"

struct nextDronePosition : Request {
    nextDronePosition(){
        this->RequestType = MESSAGE_TYPE::NEXTDRONEPOSITION;
    }
};


#endif // __NEXTDRONEPOSITION_H__
