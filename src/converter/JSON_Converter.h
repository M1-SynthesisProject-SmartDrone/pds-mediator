#if !defined(__JSON_CONVERTER_H__)
#define __JSON_CONVERTER_H__

#include "Abstract_Converter.h"

using namespace std;

class JSON_Converter : Abstract_Converter {

private:

public:

JSON_Converter();

~JSON_Converter();

Request* convertReceivedRequest(string request);

string convertToSendRequest(Response* response);

};

#endif // __JSON_CONVERTER_H__
