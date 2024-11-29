#include "config.h"
#include "buzzer.h"
#include "button.h"
#include "display.h"
#include "mqtt_client.h"
#include "wifi.h"

// Global Object Instances
Buzzer buzzer(BUZZER_PIN);
Display display(SCREEN_WIDTH, SCREEN_HEIGHT);
WiFiManager wifiManager(ssid, password); // WiFi manager instance
MQTTClient mqttClient(mqttServer, mqttPort, mqttUsername, mqttPassword); // MQTT client instance
void startQRDisplay();

// Button Instance
Button button(
    BUTTON_PIN,
    []() { // Single Click: Show QR Code
        startQRDisplay(); // Call the function to display the QR code
    },
    []() { // Double Click: Clear Display -> now also QR code
        //display.clear();
        //buzzer.playNotification();
        startQRDisplay();
    }
);

// QR Code State Variables
bool isQRDisplaying = false;
unsigned long qrDisplayEndTime = 0;
bool isNotificationActive = false;  // Track whether a notification is active

// Function to start QR Code Display
void startQRDisplay() {
    if (!isQRDisplaying && !isNotificationActive) {
        display.showQRCode("https://unstable.com");
        buzzer.playNotification();
        isQRDisplaying = true;
        qrDisplayEndTime = millis() + 30000; // Display QR for 30 seconds
    }
}

// Setup Function
void setup() {
    Serial.begin(115200);

    // Initialize components
    buzzer.playNotification();
    display.begin();
    display.showStartupCool("Initialising", "unsTable: Linak Deskline");

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

    // Handle QR Code display timeout and notification logic
    if (isQRDisplaying) {
        if (millis() > qrDisplayEndTime) {
            // If QR code time is up, clear it
            display.clear();
            isQRDisplaying = false;
        }
    }

    // If a notification comes, interrupt QR code display
    if (isNotificationActive) {
        display.showText("New Notification!");
        // If you want to keep it longer, adjust the condition here.
        isNotificationActive = false;
    }
}