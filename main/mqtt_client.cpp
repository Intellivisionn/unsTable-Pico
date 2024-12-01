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
    DynamicJsonDocument doc(200); // Use DynamicJsonDocument to avoid deprecation warning
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return;
    }

    // Extract data
    int action = doc["action"];
    const char* content = doc["content"];

    // Clear any active notification before handling the new message
    clearNotification();

    // Perform actions based on `action` value
    switch (action) {
        case 0: // User connecting
            if (content) {
                if (buzzer) buzzer->playNotification();
                String welcomeMessage = String("Hello, ") + content;
                display->showText(welcomeMessage.c_str());
                display->startTimer();
                isNotificationActive = true; // Mark notification as active
                notificationEndTime = millis() + 10000; // Show for 10 seconds
            }
            break;

        case 1: // Alert
            if (content) {
                if (buzzer) buzzer->playNotification();
                String alertMessage = content;
                display->showText(alertMessage.c_str());
                isNotificationActive = true; // Mark notification as active
                notificationEndTime = millis() + 15000; // Show for 60 seconds
            }
            break;

        case 2: // User disconnecting
            display->showText("Goodbye!");
            if (buzzer) buzzer->playNotification();
            display->stopTimer();
            isNotificationActive = true; // Mark notification as active
            notificationEndTime = millis() + 2000; // Show for 2 seconds
            break;

        case 3: // Toggle Notifications
            if (content && buzzer) {
                enableNotifications = strcmp(content, "True") == 0;
                buzzer->setEnabled(enableNotifications);
                if (display) {
                    display->showText(enableNotifications ? "Notifications Enabled" : "Notifications Disabled");
                    if (enableNotifications && buzzer) buzzer->playNotification(); // Trigger sound if notifications are enabled
                    isNotificationActive = true;
                    notificationEndTime = millis() + 2000; // Show for 2 seconds
                }
            }
            break;

        case 4: // Custom action
        {
            String topic = String(mqttTopic) + "timer"; // Create the topic
            const char* topicCStr = topic.c_str(); // Explicitly convert to const char*
            String timerDetails = String(display->getTimerDetails()); // Get timer details as String
            const char* timerDetailsCStr = timerDetails.c_str(); // Convert to const char*            MQTTClient::publishMessage(topic, String(display->getTimerDetails()).c_str());
            
            publishMessage(topicCStr, timerDetailsCStr);
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