#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h> //mqtt
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <qrcode.h>
#include <OneButton.h>

// Definitions

// WiFi
  const char* ssid = "The LAN of Svens";
  const char* password = "28449979000";
// HiveMQ Cloud configuration
  const char* mqttServer = "2b2ceeac5b834b68916739afa28908c6.s1.eu.hivemq.cloud";
  const int mqttPort = 8883; // TLS MQTT Port
  const char* mqttUsername = "PicoW-1"; // HiveMQ username
  const char* mqttPassword = "unsTable-Best-Group@1"; // HiveMQ password
// Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // DisplayInit
// Table ID
  const char* mqttTopic = "1"; // Topic to subscribe to
// MQTT WiFi Initialisations
WiFiClientSecure secureClient; // Secure WiFi client
PubSubClient mqttClient(secureClient);
//Button
#define BUTTON_PIN 10
OneButton button(BUTTON_PIN, true); // true for internal pull-up -> Create a OneButton object

// Buzzer Configuration
#define BUZZER_PIN 20

// Generate a tone on the given pin
void tone(uint8_t pin, unsigned int frequency, unsigned long duration) {
    pinMode(pin, OUTPUT); // Set the pin as output
    unsigned long halfPeriod = 1000000L / (frequency * 2);
    unsigned long cycles = frequency * duration / 1000;

    for (unsigned long i = 0; i < cycles; i++) {
        digitalWrite(pin, HIGH);
        delayMicroseconds(halfPeriod);
        digitalWrite(pin, LOW);
        delayMicroseconds(halfPeriod);
    }
}

// Stop the tone on the given pin
void noTone(uint8_t pin) {
    digitalWrite(pin, LOW); // Ensure the pin is low
}

// Function for notification sound
void notificationSound() {
    tone(BUZZER_PIN, 1000, 200); // Play 1 kHz tone for 200 ms
    delay(300); // Wait for 300 ms
    tone(BUZZER_PIN, 1500, 200); // Play 1.5 kHz tone for 200 ms
    delay(300); // Wait for 300 ms
    noTone(BUZZER_PIN);         // Stop the tone
}

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

void controlDisplay(const char* command, bool dir = false, float speed = 0.0f, const char* message = nullptr) {
    if (strcmp(command, "clear") == 0) {
        display.clearDisplay();
        display.display();
        Serial.println("Display cleared.");
    } 
    else if (strcmp(command, "fill") == 0) {
        display.fillScreen(SSD1306_WHITE);
        display.display();
        Serial.println("Display filled.");
    } 
    else if (strcmp(command, "print_motor") == 0) {
        display.clearDisplay();
        display.setTextSize(1); // Set text size
        display.setTextColor(SSD1306_WHITE); // Set text color
        display.setCursor(0, 0); // Set cursor to top-left
        display.println("Motor Control:");
        display.printf("Dir: %d\n", dir);
        display.printf("Speed: %.2f\n", speed);
        display.display();
        Serial.printf("Motor display updated with Dir: %d, Speed: %.2f.\n", dir, speed);
    } 
    else if (strcmp(command, "print_message") == 0) {
        if (message) {
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 0);
            display.println(message);
            display.display();
            Serial.printf("Message displayed: %s\n", message);
        } else {
            Serial.println("Error: No message provided.");
        }
    } 
    else {
        Serial.printf("Invalid command: %s\n", command);
    }
}

void clearDisplayWrapper() {
    display.clearDisplay();
    display.display();
    Serial.println("Display cleared via double-click.");
    notificationSound();
}

void QRCodeWrapper() {
  display.clearDisplay();
  displayQRCode("https://unstable.com");
  Serial.println("QR COde Displayed.");
  notificationSound();
}

void displayQRCode(const char* content) {
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    qrcode_initText(&qrcode, qrcodeData, 3, ECC_LOW, content);

    display.clearDisplay();

    int offset_x = (SCREEN_WIDTH - qrcode.size * 2) / 2;
    int offset_y = (SCREEN_HEIGHT - qrcode.size * 2) / 2;

    for (uint8_t y = 0; y < qrcode.size; y++) {
        for (uint8_t x = 0; x < qrcode.size; x++) {
            if (qrcode_getModule(&qrcode, x, y)) {
                display.fillRect(offset_x + x * 2, offset_y + y * 2, 2, 2, SSD1306_WHITE);
            }
        }
    }
    display.display();
}


void setup() {
  Serial.begin(115200);

      // Start the display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Correct usage
          Serial.println("SSD1306 allocation failed");
        for (;;); // Infinite loop to halt execution
    }

  printAllWiFis();

  connectToWiFi(ssid, password);

  secureClient.setInsecure();
  // Connect to MQTT
  connectToMQTT();
  
  // Configure button actions
  button.attachClick(QRCodeWrapper);
  button.attachDoubleClick(clearDisplayWrapper);

  QRCodeWrapper();
  
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

  button.tick();
}