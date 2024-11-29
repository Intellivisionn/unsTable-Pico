#include "config.h"
#include "buzzer.h"
#include "button.h"
#include "display.h"
#include "mqtt_client.h"

// Global Object Instances
Buzzer buzzer(BUZZER_PIN);
Display display(SCREEN_WIDTH, SCREEN_HEIGHT);
MQTTClient mqttClient(mqttServer, mqttPort, mqttUsername, mqttPassword);
Button button(BUTTON_PIN, []() {
    display.showQRCode("https://unstable.com");
    buzzer.playNotification();
}, []() {
    display.clear();
    buzzer.playNotification();
});

// Wi-Fi Manager
void connectToWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to Wi-Fi...");
    }
    Serial.println("Wi-Fi connected!");
}

void setup() {
    Serial.begin(115200);

    // Initialize components
    buzzer.playNotification(); // Startup sound
    display.begin();
    connectToWiFi();
    mqttClient.connect("PicoClient", mqttTopic, [](char* topic, byte* payload, unsigned int length) {
        String message;
        for (unsigned int i = 0; i < length; i++) {
            message += (char)payload[i];
        }
        display.showText(("MQTT: " + message).c_str());
    });
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        connectToWiFi();
    }

    if (!mqttClient.connected()) {
        mqttClient.connect("PicoClient", mqttTopic, [](char* topic, byte* payload, unsigned int length) {
            String message;
            for (unsigned int i = 0; i < length; i++) {
                message += (char)payload[i];
            }
            display.showText(("MQTT: " + message).c_str());
        });
    }

    mqttClient.loop();
    button.tick();
}
