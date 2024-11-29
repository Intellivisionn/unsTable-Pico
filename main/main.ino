#include "config.h"
#include "buzzer.h"
#include "button.h"
#include "display.h"
#include "mqtt_client.h"
#include "wifi.h"

// Global Object Instances
Buzzer buzzer(BUZZER_PIN);
Display display(SCREEN_WIDTH, SCREEN_HEIGHT);
MQTTClient mqttClient(mqttServer, mqttPort, mqttUsername, mqttPassword);
WiFiManager wifiManager(ssid, password); // WiFi manager instance

// Forward declaration
void startQRDisplay();

// Button Instance
Button button(
    BUTTON_PIN,
    []() { // Single Click: Show QR Code
        startQRDisplay();
    },
    []() { // Double Click: Clear Display
        display.clear();
        buzzer.playNotification();
    }
);

// QR Code State Variables
bool isQRDisplaying = false;
unsigned long qrDisplayEndTime = 0;

// Function to start QR Code Display
void startQRDisplay() {
    if (!isQRDisplaying) {
        display.showQRCode("https://unstable.com");
        buzzer.playNotification();
        isQRDisplaying = true;
        qrDisplayEndTime = millis() + 30000; // Display for 30 seconds
    }
}

// Setup Function
void setup() {
    Serial.begin(115200);

    // Initialize components
    buzzer.playNotification();
    display.begin();
    mqttClient.setDisplay(&display);
    mqttClient.setBuzzer(&buzzer);

    // Connect to Wi-Fi
    wifiManager.connectToWiFi();

    // Connect to MQTT broker
    mqttClient.connect("PicoClient", mqttTopic);
}

// Loop Function
void loop() {
    // Reconnect Wi-Fi if disconnected
    if (!wifiManager.isConnected()) {
        Serial.println("Wi-Fi disconnected. Reconnecting...");
        wifiManager.connectToWiFi();
    }

    // Reconnect MQTT if disconnected
    if (!mqttClient.isConnected()) {
        Serial.println("MQTT disconnected. Reconnecting...");
        mqttClient.connect("PicoClient", mqttTopic);
    }

    // Process MQTT messages
    mqttClient.loop();

    // Process button actions
    button.tick();

    // Handle QR Code display timeout
    if (isQRDisplaying) {
        if (millis() > qrDisplayEndTime || mqttClient.notificationActive()) {
            display.clear();
            isQRDisplaying = false;
        }
    }
}