#ifndef __MESSAGETYPE_H__
#define __MESSAGETYPE_H__

#include <loguru/loguru.hpp>
typedef enum {
    // RECEIVED MESSAGE TYPES
    REQUEST,
    TR_LAUNCH,
    END_TR_LAUNCH,
    TR_SAVE,
    END_TR_SAVE,
    REGISTER,
    // TO SEND MESSAGE TYPES
    RESP_REQUEST,
    RESP_TR_LAUNCH,
    RESP_END_TR_LAUNCH,
    RESP_TR_SAVE,
    RESP_END_TR_SAVE,
    // GLOBAL
    UNKNOWN
} MESSAGE_TYPE;


const std::vector<std::string> REQUESTTYPES {
    "REQUEST", 
    "TR_LAUNCH", 
    "END_TR_LAUNCH",
    "TR_SAVE", 
    "END_TR_SAVE", 
    "REGISTER",
    "RESP_REQUEST",
    "RESP_TR_LAUNCH", 
    "RESP_END_TR_LAUNCH", 
    "RESP_TR_SAVE",
    "RESP_END_TR_SAVE", 
    "UNKNOWN"
};


#endif // __MESSAGETYPE_H__