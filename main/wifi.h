#ifndef WIFI_H
#define WIFI_H

#include <WiFi.h>

class WiFiManager {
private:
    const char* ssid;
    const char* password;

public:
    // Constructor
    WiFiManager(const char* ssid, const char* password);

    // Methods
    void connectToWiFi();
    bool isConnected();
};

#endif
