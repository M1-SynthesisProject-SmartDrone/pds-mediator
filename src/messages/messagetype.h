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
    "RESP_REQUEST",
    "RESP_TR_LAUNCH", 
    "RESP_END_TR_LAUNCH", 
    "RESP_TR_SAVE",
    "RESP_END_TR_SAVE", 
    "UNKNOWN"
};

// struct MessageTypes{

// static int getValueOfRequestType(std::string reqtype){
//     LOG_F(INFO,"%s --- %s", reqtype.c_str() , REQUESTTYPES[MESSAGE_TYPE::REQUEST].c_str());
//     if(reqtype ==  REQUESTTYPES[MESSAGE_TYPE::REQUEST]){
//         return MESSAGE_TYPE::REQUEST;
//     }
//     // TODO to complete
//     else{
//         return MESSAGE_TYPE::UNKNOWN;
//     }
// }

// };


// std::string getStringFromEnum(MESSAGE_TYPE e){
//   switch (e)
//   {
//     case REQUEST: return "REQUEST";
//     case TR_LAUNCH: return "TR_LAUNCH";
//     case END_TR_LAUNCH: return "END_TR_LAUNCH";
//     default: return "UNKNOWN";
//   }
// }

#endif // __MESSAGETYPE_H__