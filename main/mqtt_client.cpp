#include "mqtt_client.h"
#include <ArduinoJson.h>

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

    // Parse the JSON payload
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return;
    }

    // Extract data
    int action = doc["action"];
    const char* content = doc["content"];

    // Perform actions based on `action` value
    switch (action) {
        case 0: // User connecting
            if (content) {
                String welcomeMessage = String("Hello, ") + content;
                display.showText(welcomeMessage.c_str());

                // Start counting time (implement in the display or main logic)
                display.startTimer();
            }
            break;

        case 1: // Alert
            if (content) {
                String alertMessage = String("ALERT: ") + content;
                display.showText(alertMessage.c_str());

                // Clear the alert after 1 minute
                delay(60000);
                display.clearAlert();
            }
            break;

        case 2: // User disconnecting
            display.showText("Goodbye!");
            delay(2000);
            display.clear();
            break;

        case 3: // Enable/Disable notifications
            if (content) {
                bool enableNotifications = strcmp(content, "True") == 0;
                buzzer.setEnabled(enableNotifications);
                String notifMessage = enableNotifications ? "Notifications Enabled" : "Notifications Disabled";
                display.showText(notifMessage.c_str());
                delay(2000);
                display.clear();
            }
            break;

        default:
            Serial.println("Unknown action received.");
            break;
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