#if !defined(__JSON_CONVERTER_H__)
#define __JSON_CONVERTER_H__

#include "Abstract_Converter.h"
#include <nlohmann/json.hpp>
#include "../messages/response/StartTripSaveResponse.h"
#include "../messages/response/EndTripSaveResponse.h"
#include "../messages/response/dataRegisterResponse.h"
#include "../messages/response/tripLaunchResponse.h"
#include "../messages/response/respErrorNotif.h"
#include "../messages/response/onePathResponse.h"



using namespace std;

class JSON_Converter : Abstract_Converter {

private:

nlohmann::json convertStartTripSaveResponse(StartTripSaveResponse* response);

nlohmann::json convertEndTripSaveResponse(EndTripSaveResponse* response);

nlohmann::json convertRespRegister(DataRegisterResponse* response);

nlohmann::json convertRespTripLaunch(TripLaunchResponse* response);

nlohmann::json convertRespTripLaunch(respErrorNotif* response);

nlohmann::json convertOnePathResponse(onePathResponse* response);


public:

JSON_Converter();

~JSON_Converter();

Request* convertReceivedRequest(string request);

nlohmann::json convertToSendRequest(Response* response);

};

#endif // __JSON_CONVERTER_H__
