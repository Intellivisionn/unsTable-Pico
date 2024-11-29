#include "mqtt_client.h"

MQTTClient::MQTTClient(const char* server, int port, const char* username, const char* password)
    : mqttClient(secureClient) {
    mqttClient.setServer(server, port);
    secureClient.setInsecure(); // For simplicity
}

void MQTTClient::connect(const char* clientId, const char* topic, void (*callback)(char*, byte*, unsigned int)) {
    mqttClient.setCallback(callback);
    while (!mqttClient.connected()) {
        Serial.print("Connecting to MQTT...");
        if (mqttClient.connect(clientId)) {
            Serial.println("Connected!");
            mqttClient.subscribe(topic);
        } else {
            Serial.print("Failed with state ");
            Serial.println(mqttClient.state());
            delay(5000);
        }
    }
}

bool MQTTClient::isConnected() {
    return mqttClient.connected(); // Wraps PubSubClient's connected() method
}

void MQTTClient::loop() {
    mqttClient.loop();
}
