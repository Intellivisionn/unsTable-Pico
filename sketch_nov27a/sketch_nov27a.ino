#include <WiFi.h>

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

void setup() {
  Serial.begin(115200);

  printAllWiFis();

  const char* ssid = "78463212";
  const char* password = "78463212";

  connectToWiFi(ssid, password);
}

void loop() {}