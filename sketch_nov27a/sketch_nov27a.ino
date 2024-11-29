#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Definitions

// WiFi
  const char* ssid = "The LAN of Svens";
  const char* password = "28449979000";
// HiveMQ Cloud configuration
  const char* mqttServer = "2b2ceeac5b834b68916739afa28908c6.s1.eu.hivemq.cloud";
  const int mqttPort = 8883; // TLS MQTT Port
  const char* mqttUsername = "PicoW-1"; // HiveMQ username
  const char* mqttPassword = "unsTable-Best-Group@1"; // HiveMQ password
// Table ID
  const char* mqttTopic = "1"; // Topic to subscribe to

WiFiClientSecure secureClient; // Secure WiFi client
PubSubClient mqttClient(secureClient);

void printAllWiFis() {
  delay(1000);

  Serial.println("Scanning for Wi-Fi networks...");

  // Start Wi-Fi in station mode
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(); // Disconnect from any previous connection
  delay(1000);

  int networkCount = WiFi.scanNetworks();

  if (networkCount == 0) {
    Serial.println("No networks found.");
  } else {
    Serial.println("Networks found:");
    for (int i = 0; i < networkCount; i++) {
      // Print network SSID, RSSI, and encryption type
      Serial.printf(
        "%d: %s (RSSI: %d) Encryption: %d\n",
        i + 1,
        WiFi.SSID(i),
        WiFi.RSSI(i),
        WiFi.encryptionType(i)
      );
      delay(10);
    }
  }

  Serial.println("Scan complete.");
}

void connectToWiFi(const char* ssid, const char* password) {
  // Begin Wi-Fi in station mode
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");

  // Wait until the device connects
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    attempts++;

    // Timeout after 20 attempts (20 seconds)
    if (attempts > 20) {
      Serial.println("\nFailed to connect to Wi-Fi. Exiting...");
      exit(0);
    }
  }

  // Wi-Fi connected
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message: ");
  Serial.println(message);
}

void connectToMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback(mqttCallback);

  // Start connection
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect("PicoClient", mqttUsername, mqttPassword)) {
      Serial.println("Connected to MQTT broker!");
      mqttClient.subscribe(mqttTopic);
      Serial.print("Subscribed to topic: ");
      Serial.println(mqttTopic);
    } else {
      Serial.print("Failed with state ");
      Serial.println(mqttClient.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);

  printAllWiFis();

  connectToWiFi(ssid, password);

  secureClient.setInsecure();
  // Connect to MQTT
  connectToMQTT();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    connectToWiFi(ssid, password);
  }

  if (!mqttClient.connected()) {
    Serial.println("MQTT disconnected. Reconnecting...");
    connectToMQTT();
  }

  mqttClient.loop(); // Process incoming MQTT messages
}