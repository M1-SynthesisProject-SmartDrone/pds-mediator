#if !defined(__ABSTRACT_CONVERTER_H__)
#define __ABSTRACT_CONVERTER_H__

#include "../messages/request/Request.h"
#include "../messages/response/Response.h"
#include <nlohmann/json.hpp>

using namespace std;

class Abstract_Converter
{
private:

public:

    Request* convertReceivedRequest(string request);

    nlohmann::json convertToSendRequest(Response* response);
};



#endif // __ABSTRACT_CONVERTER_H__
