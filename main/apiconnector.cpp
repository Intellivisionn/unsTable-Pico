#include "APIConnector.h"
#include <HTTPClient.h>

// Constructor
APIConnector::APIConnector() {
    // Set the base URL and API key from config.h
    baseUrl = String(SUPABASE_URL);
    apiKey = String(SUPABASE_API_KEY);
}

// Destructor
APIConnector::~APIConnector() {}

// Helper method to set headers for HTTP requests
void APIConnector::setHeaders(HTTPClient& http) {
    http.addHeader("Content-Type", "application/json");
    http.addHeader("apikey", apiKey); // Supabase requires an API key header
}

// Perform an HTTP GET request
String APIConnector::get(const String& endpoint) {
    HTTPClient http;
    String url = baseUrl + endpoint;
    http.begin(url);
    setHeaders(http);

    int httpResponseCode = http.GET();
    String response = (httpResponseCode > 0) ? http.getString() : "{}";
    http.end();

    if (httpResponseCode <= 0) {
        Serial.print("GET request failed. HTTP response code: ");
        Serial.println(httpResponseCode);
    }

    return response;
}

// Perform an HTTP POST request
String APIConnector::post(const String& endpoint, const String& payload) {
    HTTPClient http;
    String url = baseUrl + endpoint;
    http.begin(url);
    setHeaders(http);

    int httpResponseCode = http.POST(payload);
    String response = (httpResponseCode > 0) ? http.getString() : "{}";
    http.end();

    if (httpResponseCode <= 0) {
        Serial.print("POST request failed. HTTP response code: ");
        Serial.println(httpResponseCode);
    }

    return response;
}

// Perform an HTTP PUT request
String APIConnector::put(const String& endpoint, const String& payload) {
    HTTPClient http;
    String url = baseUrl + endpoint;
    http.begin(url);
    setHeaders(http);

    int httpResponseCode = http.PUT(payload);
    String response = (httpResponseCode > 0) ? http.getString() : "{}";
    http.end();

    if (httpResponseCode <= 0) {
        Serial.print("PUT request failed. HTTP response code: ");
        Serial.println(httpResponseCode);
    }

    return response;
}

// Perform an HTTP DELETE request
String APIConnector::del(const String& endpoint) {
    HTTPClient http;
    String url = baseUrl + endpoint;
    http.begin(url);
    setHeaders(http);

    int httpResponseCode = http.sendRequest("DELETE");
    String response = (httpResponseCode > 0) ? http.getString() : "{}";
    http.end();

    if (httpResponseCode <= 0) {
        Serial.print("DELETE request failed. HTTP response code: ");
        Serial.println(httpResponseCode);
    }

    return response;
}