#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "display.h"

class MQTTClient {
private:
    WiFiClientSecure secureClient;
    PubSubClient mqttClient;

    const char* mqttUsername;
    const char* mqttPassword;
    const char* topic;
    Display* display;

    static MQTTClient* instance; // Singleton instance for callback routing

    // Internal callback handler
    void messageCallback(char* topic, byte* payload, unsigned int length);

public:
    MQTTClient(const char* server, int port, const char* username, const char* password);

    void connect(const char* clientId, const char* topic);
    void loop();
    bool isConnected();

    void setDisplay(Display* display);

    // Static callback function
    static void staticCallback(char* topic, uint8_t* payload, unsigned int length);
};

#endif