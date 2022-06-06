#if !defined(__ONEPATHRESPONSE_H__)
#define __ONEPATHRESPONSE_H__

#include "Response.h"
#include "../messagetype.h"

struct onePathResponse : Response {
    std::string name;
    int id;
    int nbPoints;
    int nbCheckpoints;
    int date;
    int latitude;
    int longitude;
    int altitude;

    onePathResponse(std::string name, int id, int nbPoints, int nbCheckpoints, int date, int latitude, int longitude, int altitude){
        this->ResponseType = MESSAGE_TYPE::RESP_ONE_PATH;
        this->name = name;
        this->nbPoints = nbPoints;
        this->nbCheckpoints = nbCheckpoints;
        this->date = date;
        this->latitude = latitude;
        this->longitude = longitude;
        this->altitude = altitude;
    }
};

#endif // __ONEPATHRESPONSE_H__
