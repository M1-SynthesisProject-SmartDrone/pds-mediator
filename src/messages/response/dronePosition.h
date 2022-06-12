#if !defined(__DRONEPOSITION_H__)
#define __DRONEPOSITION_H__

#include "Response.h"

struct dronePosition : Response{

    int imagesize;
    int id_pos;
    dronePosition(int imageSize, int id_post){
        this->ResponseType = MESSAGE_TYPE::DRONEPOSITION;
        this->imagesize = imageSize;
        this->id_pos = id_pos;
    }

};

#endif // __DRONEPOSITION_H__
