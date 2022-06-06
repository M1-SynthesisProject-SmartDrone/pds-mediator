#if !defined(__GETPATHLIST_H__)
#define __GETPATHLIST_H__

#include "Request.h"
#include "../messagetype.h"

struct getPathList : Request {

    getPathList(){
        this->RequestType = MESSAGE_TYPE::GET_PATH_LIST ;
    }
};


#endif // __GETPATHLIST_H__
