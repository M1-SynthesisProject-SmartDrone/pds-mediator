#ifndef __DATAREQUEST_H__
#define __DATAREQUEST_H__

#include "Request.h"
#include "../messagetype.h"

using namespace std;

struct DataRequest : Request {

    string valueType;
    int tr_id;
    int position_id;

    DataRequest(string valueType, int tr_id, int position_id){
        this->RequestType = MESSAGE_TYPE::REQUEST;
        this->valueType = valueType;
        this->tr_id = tr_id;
        this->position_id = position_id;
    }

};


#endif