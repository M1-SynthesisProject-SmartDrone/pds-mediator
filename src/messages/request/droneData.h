#if !defined(__DRONEDATA_REGISTER_H__)
#define __DRONEDATA_REGISTER_H__

#include "Request.h"
#include "../messagetype.h"
#include <loguru/loguru.hpp>

using namespace std;

struct DroneDataRegister : Request{

    int tr_id;
    int pointId;
    int altitude;
    int latitude;
    int longitude;
    int rotation;
    bool isCheckpoint;
    float pressure;
    float temperature;
    int batteryRemaining;
    time_t date;
    ssize_t image;


    DroneDataRegister(int tr_id, int pointId, int altitude, int latitude, int longitude, int rotation, bool isCheckpoint, float pressure, float temperature, int batteryRemaining, time_t date, int image){
        this->RequestType = MESSAGE_TYPE::REGISTER;
        this->tr_id = tr_id;
        this->pointId = pointId;
        this->altitude = altitude;
        this->latitude = latitude;
        this->longitude = longitude;
        this->rotation = rotation;
        this->isCheckpoint = isCheckpoint;
        this->pressure = pressure;
        this->temperature = temperature;
        this->batteryRemaining = batteryRemaining;
        this->date = date;
        this->image=image;
    }


};

#endif // __DRONEDATA_REGISTER_H__
