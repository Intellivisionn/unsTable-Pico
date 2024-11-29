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

    // Optional reference to a Display object for showing messages
    Display* display;

    // Internal callback handler
    void messageCallback(char* topic, byte* payload, unsigned int length);

public:
    MQTTClient(const char* server, int port, const char* username, const char* password);

    // Methods
    void connect(const char* clientId, const char* topic);
    void loop();
    bool isConnected();

    // Setters
    void setDisplay(Display* display);
};

#endif