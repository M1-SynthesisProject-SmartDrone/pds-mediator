#if !defined(__RESPERRORNOTIF_H__)
#define __RESPERRORNOTIF_H__

#include "Response.h"
#include "../messagetype.h"

struct respErrorNotif : Response {
    respErrorNotif(){
        this->ResponseType = MESSAGE_TYPE::ERROR_NOTIFICATION_RECEIVED;
    }
};

#endif // __RESPERRORNOTIF_H__
