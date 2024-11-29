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
WiFiManager wifiManager(ssid, password);
Button button(
    BUTTON_PIN,
    []() { // Single Click: Show QR Code
        display.showQRCode("https://unstable.com");
        buzzer.playNotification();
        delay(5000);
        display.clear();
    },
    []() { // Double Click: Clear Display
        display.clear();
        buzzer.playNotification();
    }
);

void setup() {
    Serial.begin(115200);

    // Initialize the display
    display.begin();
    display.clearAndDisplay();
    Serial.println("Display initialized!");

    // Startup buzzer notification
    buzzer.playNotification();

    // Connect to Wi-Fi
    wifiManager.connectToWiFi();

    // Set the display for MQTT messages
    mqttClient.setDisplay(&display);

    // Connect to MQTT broker
    mqttClient.connect("PicoClient", mqttTopic);
}

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
}