#if !defined(__GETONEPATH_H__)
#define __GETONEPATH_H__

#include "Request.h"
#include "../messagetype.h"

struct getOnePath : Request {

    int tr_id;
    getOnePath(int tr_id){
        this->tr_id = tr_id;
        this->RequestType = MESSAGE_TYPE::GET_ONE_PATH ;
    }
};


#endif // __GETONEPATH_H__
