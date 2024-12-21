#include "mqtt_client.h"
#include <ArduinoJson.h>
#include "config.h"

// Define the static instance member
MQTTClient* MQTTClient::instance = nullptr;

// Constructor
MQTTClient::MQTTClient(const char* server, int port, const char* username, const char* password)
    : mqttClient(secureClient), mqttUsername(username), mqttPassword(password), display(nullptr), buzzer(nullptr),
      enableNotifications(false), notificationEndTime(0), isNotificationActive(false) {
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

    clearNotification(); // Clear the current notification    

    // Perform actions based on `action` value
    switch (action) {
        case 0: // User connecting
            {
                // Extract nested values from `content`
                const char* username = content["username"];
                enableNotifications = content["notifications"];
                buzzer->setEnabled(enableNotifications);
                unsigned long offsetMillis = content["offset"];
                standupReminderTime = content["standupTime"];
                breakReminderTime = content["breakTime"];

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
                scheduleNotifications();
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
                display->stopTimer();
                display->startTimer(offsetMillis);
            }
            break;

        case 3: // User settings
            if (buzzer) {
                enableNotifications = content["notifications"];
                buzzer->setEnabled(enableNotifications);
                standupReminderTime = content["standupTime"];
                breakReminderTime = content["breakTime"];
                scheduleNotifications();
            }
            break;

        default:
            Serial.println("Unknown action received.");
            break;
    }
}

// Schedule notifications based on the reminder times
void MQTTClient::scheduleNotifications() {
    if (enableNotifications) {
        unsigned long displayTimeMillis = 0;

        if (standupReminderTime > 0) {
            nextStandupTime = displayTimeMillis + standupReminderTime * 60000; // Add minutes to current display time
            Serial.print("Next standup reminder scheduled at display time (ms): ");
            Serial.println(nextStandupTime);
        }

        if (breakReminderTime > 0) {
            nextBreakTime = displayTimeMillis + breakReminderTime * 60000; // Add minutes to current display time
            Serial.print("Next break reminder scheduled at display time (ms): ");
            Serial.println(nextBreakTime);
        }
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

void MQTTClient::loop() {
    mqttClient.loop();

    if (!enableNotifications) {
        return; // Notifications are disabled
    }

    unsigned long displayTimeMillis = display->getElapsedTime(); // Current elapsed time from display

    // Check for combined notification
    if (standupReminderTime > 0 && breakReminderTime > 0 && displayTimeMillis >= nextStandupTime && displayTimeMillis >= nextBreakTime) {
        if (display) {
            display->showText("Time for a break and to put table up!");
            notificationEndTime = millis() + 10000; // 10 seconds
            isNotificationActive = true;
        }
        if (buzzer) {
            buzzer->playNotification();
        }
        Serial.println("Combined standup and break reminder triggered.");
        nextStandupTime += standupReminderTime * 60000; // Reschedule the next standup reminder
        nextBreakTime += breakReminderTime * 60000;     // Reschedule the next break reminder
        Serial.print("Next combined reminder rescheduled to: ");
        Serial.println(nextStandupTime);
        return; // Exit to avoid triggering separate notifications below
    }

    // Trigger standup reminder if the elapsed display time has reached or passed the next scheduled time
    if (standupReminderTime > 0 && displayTimeMillis >= nextStandupTime) {
        if (display) {
            display->showText("Time to put the table up!");
            notificationEndTime = millis() + 10000; // 10 seconds
            isNotificationActive = true;
        }
        if (buzzer) {
            buzzer->playNotification();
        }
        Serial.println("Standup reminder triggered.");
        nextStandupTime += standupReminderTime * 60000; // Schedule the next standup reminder
        Serial.print("Next standup reminder rescheduled to: ");
        Serial.println(nextStandupTime);
    }

    // Trigger break reminder if the elapsed display time has reached or passed the next scheduled time
    if (breakReminderTime > 0 && displayTimeMillis >= nextBreakTime) {
        if (display) {
            display->showText("Time for a break!");
            notificationEndTime = millis() + 10000; // 10 seconds
            isNotificationActive = true;
        }
        if (buzzer) {
            buzzer->playNotification();
        }
        Serial.println("Break reminder triggered.");
        nextBreakTime += breakReminderTime * 60000; // Schedule the next break reminder
        Serial.print("Next break reminder rescheduled to: ");
        Serial.println(nextBreakTime);
    }

    // Clear notifications if the active time has passed
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