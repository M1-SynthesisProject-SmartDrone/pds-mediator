#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include <string>

#include <nlohmann/json.hpp>
#include "../messagetype.h"

struct Response
{
    
    MESSAGE_TYPE ResponseType;

};

#endif // __RESPONSE_H__