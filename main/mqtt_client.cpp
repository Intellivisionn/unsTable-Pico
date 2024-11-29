#include "mqtt_client.h"

// Initialize static instance pointer
MQTTClient* MQTTClient::instance = nullptr;

// Constructor
MQTTClient::MQTTClient(const char* server, int port, const char* username, const char* password)
    : mqttClient(secureClient), mqttUsername(username), mqttPassword(password) {
    mqttClient.setServer(server, port);
    secureClient.setInsecure(); // Simplify TLS handling
    display = nullptr;          // Ensure display reference is null by default
    instance = this;            // Assign this instance to the static pointer
}

// Connect to the MQTT broker
void MQTTClient::connect(const char* clientId, const char* topic) {
    this->topic = topic; // Store the topic
    mqttClient.setCallback(staticCallback); // Set the static callback

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

// Static callback function
void MQTTClient::staticCallback(char* topic, uint8_t* payload, unsigned int length) {
    if (instance) {
        instance->messageCallback(topic, payload, length);
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