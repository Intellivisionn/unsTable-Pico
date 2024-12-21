#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "display.h"
#include "buzzer.h" // Ensure Buzzer is included

class MQTTClient {
    Buzzer* buzzer; // Add a pointer to the Buzzer class
private:
    WiFiClientSecure secureClient;
    PubSubClient mqttClient;

    unsigned long notificationEndTime; // Tracks when the current notification should end
    bool isNotificationActive;        // Tracks if a notification is currently active
    void clearNotification();         // Helper to clear the current notification

    const char* mqttUsername;
    const char* mqttPassword;
    const char* topic;
    Display* display;

    static MQTTClient* instance; // Singleton instance for callback routing

    // Internal callback handler
    void messageCallback(char* topic, byte* payload, unsigned int length);

    bool enableNotifications = true; // Default to notifications enabled
    int standupReminderTime = 120;   // Default standupReminderTim
    int breakReminderTime = 60;         // Default breakReminderTime

    void publishMessage(const char* topic, const char* message);

public:
    bool notificationActive() const { return isNotificationActive; }
    MQTTClient(const char* server, int port, const char* username, const char* password);
    void setBuzzer(Buzzer* buzzer);

    void connect(const char* clientId, const char* topic);
    void loop();
    bool isConnected();

    void setDisplay(Display* display);

    // Static callback function
    static void staticCallback(char* topic, uint8_t* payload, unsigned int length);
};

#endif