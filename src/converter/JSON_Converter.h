#if !defined(__JSON_CONVERTER_H__)
#define __JSON_CONVERTER_H__

#include "Abstract_Converter.h"
#include <nlohmann/json.hpp>
#include "../messages/response/StartTripSaveResponse.h"
#include "../messages/response/EndTripSaveResponse.h"


using namespace std;

class JSON_Converter : Abstract_Converter {

private:

nlohmann::json convertStartTripSaveResponse(StartTripSaveResponse* response);

nlohmann::json convertEndTripSaveResponse(EndTripSaveResponse* response);

public:

JSON_Converter();

~JSON_Converter();

Request* convertReceivedRequest(string request);

nlohmann::json convertToSendRequest(Response* response);

};

#endif // __JSON_CONVERTER_H__
