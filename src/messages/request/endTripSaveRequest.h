#if !defined(__ENDTRIPSAVEREQUEST_H__)
#define __ENDTRIPSAVEREQUEST_H__

#include "Request.h"
#include "../messagetype.h"

using namespace std;

struct endTripSaveRequest: Request
{
    string tripName;

    endTripSaveRequest(string tripName){
        this->RequestType = MESSAGE_TYPE::END_TR_SAVE;
        this->tripName = tripName;
    }
};

#endif // __ENDTRIPSAVEREQUEST_H__
