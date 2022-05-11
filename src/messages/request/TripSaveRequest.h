#if !defined(__TRIPSAVEREQUEST_H__)
#define __TRIPSAVEREQUEST_H__

#include "Request.h"
#include "../messagetype.h"

using namespace std;

struct TripSaveRequest: Request
{


    TripSaveRequest(){
        this->RequestType = MESSAGE_TYPE::TR_SAVE;
    }
    
};

#endif // __TRIPSAVEREQUEST_H__
