#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

class MQTTClient {
private:
    WiFiClientSecure secureClient;
    PubSubClient mqttClient;

    // Add these members to store MQTT credentials
    const char* mqttUsername;
    const char* mqttPassword;

public:
    MQTTClient(const char* server, int port, const char* username, const char* password);
    void connect(const char* clientId, const char* topic, void (*callback)(char*, byte*, unsigned int));
    void loop();
    bool isConnected();
};

#endif
