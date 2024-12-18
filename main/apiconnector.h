#ifndef APICONNECTOR_H
#define APICONNECTOR_H

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "config.h"

class APIConnector {
public:
    APIConnector();
    ~APIConnector();

    // Methods for HTTP operations
    String get(const String& endpoint);
    String post(const String& endpoint, const String& payload);
    String put(const String& endpoint, const String& payload);
    String del(const String& endpoint);

private:
    String baseUrl;
    String apiKey;
    void setHeaders(HTTPClient& http);
};

#endif