// APIConnector.cpp
#include "APIConnector.h"
#include <Supabase.h>

APIConnector::APIConnector() {
    // Retrieve API details from config.h
    baseUrl = String(SUPABASE_URL);
    apiKey = String(SUPABASE_API_KEY);
}

APIConnector::~APIConnector() {
    // Destructor
}

void APIConnector::setHeaders(HTTPClient& http) {
    http.addHeader("Content-Type", "application/json");
    http.addHeader("apikey", apiKey);
}

String APIConnector::get(const String& endpoint) {
    HTTPClient http;
    String url = baseUrl + endpoint;
    http.begin(url);
    setHeaders(http);

    int httpResponseCode = http.GET();
    String response = httpResponseCode > 0 ? http.getString() : "{}";
    http.end();

    return response;
}

String APIConnector::post(const String& endpoint, const String& payload) {
    HTTPClient http;
    String url = baseUrl + endpoint;
    http.begin(url);
    setHeaders(http);

    int httpResponseCode = http.POST(payload);
    String response = httpResponseCode > 0 ? http.getString() : "{}";
    http.end();

    return response;
}

String APIConnector::put(const String& endpoint, const String& payload) {
    HTTPClient http;
    String url = baseUrl + endpoint;
    http.begin(url);
    setHeaders(http);

    int httpResponseCode = http.PUT(payload);
    String response = httpResponseCode > 0 ? http.getString() : "{}";
    http.end();

    return response;
}

String APIConnector::del(const String& endpoint) {
    HTTPClient http;
    String url = baseUrl + endpoint;
    http.begin(url);
    setHeaders(http);

    int httpResponseCode = http.sendRequest("DELETE");
    String response = httpResponseCode > 0 ? http.getString() : "{}";
    http.end();

    return response;
}