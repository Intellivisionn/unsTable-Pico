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

// MQTT Callback Function
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.print("Message arrived on topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    Serial.println(message);

    // Show received message on the display
    display.showText(("MQTT: " + message).c_str());
}

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

    // Connect to MQTT broker
    mqttClient.connect("PicoClient", mqttTopic, mqttCallback);
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
        mqttClient.connect("PicoClient", mqttTopic, mqttCallback);
    }

    // Process MQTT messages
    mqttClient.loop();

    // Process button actions
    button.tick();
}
