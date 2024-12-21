#include "mqtt_client.h"
#include <ArduinoJson.h>
#include "config.h"

// Define the static instance member
MQTTClient* MQTTClient::instance = nullptr;

// Constructor
MQTTClient::MQTTClient(const char* server, int port, const char* username, const char* password)
    : mqttClient(secureClient), mqttUsername(username), mqttPassword(password), display(nullptr), buzzer(nullptr),
      enableNotifications(true), notificationEndTime(0), isNotificationActive(false) {
    mqttClient.setServer(server, port);
    secureClient.setInsecure(); // Simplify TLS handling
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
    DynamicJsonDocument doc(256); // Increased size for nested JSON
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return;
    }

    // Extract data
    int action = doc["action"];
    JsonObject content = doc["content"]; // Correctly handle `content` as an object

    Serial.print("Action: ");
    Serial.println(action);

    if (content.isNull()) {
        Serial.println("Content is missing or not an object.");
        return;
    }

    String responseTopic = String(mqttTopic) + "response";
    const char* responseTopicCStr = responseTopic.c_str();
    const char* response = "1";
    publishMessage(responseTopicCStr, response);

    isNotificationActive = false; // Reset notification flag

    // Perform actions based on `action` value
    switch (action) {
        case 0: // User connecting
            {
                // Extract nested values from `content`
                const char* username = content["username"];
                enableNotifications = content["notifications"];
                unsigned long offsetMillis = content["offset"];

                if (buzzer && enableNotifications) {
                    buzzer->playNotification();
                }

                if (username) {
                    String welcomeMessage = String("Welcome, ") + username;
                    if (display) {
                        display->showText(welcomeMessage.c_str());
                        display->startTimer(offsetMillis);
                    }
                    notificationEndTime = millis() + 10000; // 10 seconds
                    isNotificationActive = true;
                }
            }
            break;

        case 1: // User disconnecting
            if (display) {
                display->stopTimer();
                display->showText("Goodbye");
                notificationEndTime = millis() + 10000; // 10 seconds
                isNotificationActive = true;

            }
            break;

        case 2: // Update timer time
            if (display) {
                unsigned long offsetMillis = content["offset"];
                display->startTimer(offsetMillis);
            }
            break;

        case 3: // Notifications
            if (buzzer) {
                enableNotifications = content["notifications"];
            }
            break;

        default:
            Serial.println("Unknown action received.");
            break;
    }
}

// Clear the current notification
void MQTTClient::clearNotification() {
    if (isNotificationActive) {
        display->clear();
        isNotificationActive = false;
        notificationEndTime = 0;
    }
}

// Set Display Pointer
void MQTTClient::setDisplay(Display* display) {
    this->display = display;
}

// Set Buzzer Pointer
void MQTTClient::setBuzzer(Buzzer* buzzer) {
    this->buzzer = buzzer;

    if (buzzer) {
        buzzer->setEnabled(enableNotifications);
    }
}

// Check if connected to the broker
bool MQTTClient::isConnected() {
    return mqttClient.connected();
}

// Process MQTT messages
void MQTTClient::loop() {
    mqttClient.loop();

    // Check if the notification needs to be cleared
    if (isNotificationActive && millis() > notificationEndTime) {
        clearNotification();
    }
}

void MQTTClient::publishMessage(const char* topic, const char* message) {
    if (mqttClient.connected()) {
        if (mqttClient.publish(topic, message)) {
            Serial.println("Message published successfully.");
        } else {
            Serial.println("Failed to publish message.");
        }
    } else {
        Serial.println("Not connected to MQTT broker. Cannot publish message.");
    }
}