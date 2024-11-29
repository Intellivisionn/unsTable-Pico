#ifndef CONFIG_H
#define CONFIG_H

// Wi-Fi Configuration
const char* ssid = "The LAN of Svens";
const char* password = "28449979000";

// MQTT Configuration
const char* mqttServer = "2b2ceeac5b834b68916739afa28908c6.s1.eu.hivemq.cloud";
const int mqttPort = 8883;
const char* mqttUsername = "PicoW-1";
const char* mqttPassword = "unsTable-Best-Group@1";
const char* mqttTopic = "1"; // Subscription topic

// Pin Configuration
#define BUTTON_PIN 10
#define BUZZER_PIN 20

#endif
