#include "mqtt_client.h"

// Constructor
MQTTClient::MQTTClient(const char* server, int port, const char* username, const char* password)
    : mqttClient(secureClient), mqttUsername(username), mqttPassword(password) {
    mqttClient.setServer(server, port);
    secureClient.setInsecure(); // Simplify TLS handling for development
    display = nullptr;          // Ensure display reference is null by default
}

// Connect to the MQTT broker
void MQTTClient::connect(const char* clientId, const char* topic) {
    this->topic = topic; // Store the topic
    mqttClient.setCallback([this](char* topic, byte* payload, unsigned int length) {
        this->messageCallback(topic, payload, length);
    });

    while (!mqttClient.connected()) {
        Serial.print("Connecting to MQTT...");
        if (mqttClient.connect(clientId, mqttUsername, mqttPassword)) {
            Serial.println("Connected to MQTT broker!");
            mqttClient.subscribe(topic);
            Serial.println("Subscribed to topic: " + String(topic));
        } else {
            Serial.print("Failed with state ");
            Serial.println(mqttClient.state());
            delay(5000); // Retry after 5 seconds
        }
    }
}

// Internal message handler
void MQTTClient::messageCallback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.print("Message arrived on topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    Serial.println(message);

    // Display message on the screen if a display object is set
    if (display) {
        display->showText(("MQTT: " + message).c_str());
    }
}

// Check if connected to the broker
bool MQTTClient::isConnected() {
    return mqttClient.connected();
}

// Process MQTT messages
void MQTTClient::loop() {
    mqttClient.loop();
}

// Set the display object
void MQTTClient::setDisplay(Display* display) {
    this->display = display;
}