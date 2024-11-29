#include "wifi.h"

// Constructor
WiFiManager::WiFiManager(const char* ssid, const char* password)
    : ssid(ssid), password(password) {}

// Connect to Wi-Fi
void WiFiManager::connectToWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to Wi-Fi...");
    }
    Serial.println("Wi-Fi connected!");
}

// Check if Wi-Fi is connected
bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}