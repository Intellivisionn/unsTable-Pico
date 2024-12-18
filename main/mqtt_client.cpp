#include "mqtt_client.h"
#include <ArduinoJson.h>
#include "config.h"
#include "apiconnector.h"

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
              // Parse user_id from the content
              String userId = String(content);

              // Create an instance of APIConnector
              APIConnector apiConnector;

              // Fetch user settings
              String endpoint = "/settings?user_id=eq." + userId;
              String settingsResponse = apiConnector.get(endpoint);
              DynamicJsonDocument settingsDoc(512);
              deserializeJson(settingsDoc, settingsResponse);

              if (settingsDoc.size() > 0) {
                  bool notificationsEnabled = settingsDoc[0]["notifications_enabled"];
                  int standUpReminderMinutes = settingsDoc[0]["stand_up_reminder_minutes"];
                  int breakReminderMinutes = settingsDoc[0]["break_reminder_minutes"];

                  // Set notifications state
                  enableNotifications = notificationsEnabled;

                  // Fetch session data
                  String sessionEndpoint = "/sessions?table_id=eq." + String(topic) + "&user_id=eq." + userId + "&is_working_now=eq.true";
                  String sessionResponse = apiConnector.get(sessionEndpoint);
                  DynamicJsonDocument sessionDoc(512);
                  deserializeJson(sessionDoc, sessionResponse);

                  if (sessionDoc.size() > 0) {
                      String startTimeStr = sessionDoc[0]["start_time"];
                      // Convert start_time to timestamp and calculate elapsed time
                      long elapsedTime = calculateElapsedTime(startTimeStr);

                      // Update display with elapsed timer and notification
                      if (display) {
                          display->showText("Session Active");
                          display->startTimer(); // Start the timer
                          display->updateTimer(elapsedTime); // Pass the calculated elapsed time
                      }

                      Serial.print("Elapsed time: ");
                      Serial.println(elapsedTime);
                  } else {
                      Serial.println("No active session found.");
                  }

              } else {
                  Serial.println("Settings not found for user.");
              }

              if (buzzer && enableNotifications) {
                  buzzer->playNotification();
              }

              String welcomeMessage = String("Welcome, ") + content;
              display->showText(welcomeMessage.c_str());
              isNotificationActive = true;
              notificationEndTime = millis() + 10000; // 10 seconds
          }
          break;

        default:
            Serial.println("Unknown action received.");
            break;

        String topic = String(mqttTopic) + "response"; // Create the topic
        const char* topicCStr = topic.c_str(); // Explicitly convert to const char*
        const char* response = "1";
        publishMessage(topicCStr, response);
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

long MQTTClient::calculateElapsedTime(const String& startTimeStr) {
    // Parse start_time string to timestamp
    // Assuming format: "2024-12-18T12:34:56.789Z"
    tm startTime;
    sscanf(startTimeStr.c_str(), "%4d-%2d-%2dT%2d:%2d:%2d",
           &startTime.tm_year, &startTime.tm_mon, &startTime.tm_mday,
           &startTime.tm_hour, &startTime.tm_min, &startTime.tm_sec);

    startTime.tm_year -= 1900; // Adjust year
    startTime.tm_mon -= 1;     // Adjust month

    time_t startTimestamp = mktime(&startTime);
    time_t currentTimestamp = time(nullptr);

    return difftime(currentTimestamp, startTimestamp);
}
