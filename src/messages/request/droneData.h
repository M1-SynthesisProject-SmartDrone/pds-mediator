#if !defined(__DRONEDATA_REGISTER_H__)
#define __DRONEDATA_REGISTER_H__

#include "Request.h"
#include "../messagetype.h"
#include <loguru/loguru.hpp>

using namespace std;

struct DroneDataRegister : Request{

    int tr_id;
    int latitude;
    int longitude;
    int rotation;
    time_t date;
    string collectionName;
    string image;


    DroneDataRegister(int tr_id, int latitude, int longitude, int rotation, time_t date, string collectionName, string image){
        this->RequestType = MESSAGE_TYPE::REGISTER;
        this->tr_id = tr_id;
        this->latitude = latitude;
        this->longitude = longitude;
        this->rotation = rotation;
        this->date = date;
        this->collectionName = collectionName;
        this->image=image;
    }


};

#endif // __DRONEDATA_REGISTER_H__
