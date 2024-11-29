#include "mqtt_client.h"

MQTTClient::MQTTClient(const char* server, int port, const char* username, const char* password)
    : mqttClient(secureClient), mqttUsername(username), mqttPassword(password) {
    mqttClient.setServer(server, port);
    secureClient.setInsecure(); // Use insecure TLS (for simplicity)
}

void MQTTClient::connect(const char* clientId, const char* topic, void (*callback)(char*, byte*, unsigned int)) {
    mqttClient.setCallback(callback); // Set the MQTT message callback
    while (!mqttClient.connected()) {
        Serial.print("Connecting to MQTT...");
        // Pass username and password for authentication
        if (mqttClient.connect(clientId, mqttUsername, mqttPassword)) {
            Serial.println("Connected to MQTT broker!");
            mqttClient.subscribe(topic); // Subscribe to the topic after connecting
            Serial.println("Subscribed to topic: " + String(topic));
        } else {
            Serial.print("Failed with state ");
            Serial.println(mqttClient.state());
            delay(5000); // Retry after 5 seconds
        }
    }
}

bool MQTTClient::isConnected() {
    return mqttClient.connected();
}

void MQTTClient::loop() {
    mqttClient.loop();
}
