#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Replace with Supabase details
const char* baseUrl = "http://192.168.137.1:8000"; // Replace with Supabase URL
const char* apiKey = "F7H1vM3kQ5rW8zT9xG2pJ6nY4dL0aZ3K"; // Replace with API key

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Declare an SSD1306 display object connected to I2C (default address 0x3C)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Function to update the OLED display 
void updateDisplay(const String& text1, const String& text2, int shapeOption = 0) {
  // Clear the display buffer
  display.clearDisplay();

  // Set text size and color
  display.setTextSize(1);      // Small text size
  display.setTextColor(SSD1306_WHITE); // White text

  // Display the first line of text
  display.setCursor(0, 0);
  display.println(text1);

  // Display the second line of text
  display.setCursor(0, 10);
  display.println(text2);

  // Optionally draw a shape based on the shapeOption
  if (shapeOption == 1) {
    // Draw a rectangle
    display.drawRect(10, 20, 50, 30, SSD1306_WHITE);
  } else if (shapeOption == 2) {
    // Draw a circle
    display.drawCircle(90, 40, 15, SSD1306_WHITE);
  }
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

void APIGetRequest(const char* endpoint) {
  // Ensure Wi-Fi is connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi not connected!");
    return;
  }

  // Prepare the full URL for the API endpoint
  String url = String(baseUrl) + "/api/v2/" + apiKey + endpoint;

  // Create HTTP client
  HTTPClient http;

  // Begin connection
  http.begin(url);

  // Send GET request
  int httpResponseCode = http.GET();

  // Handle the response
  if (httpResponseCode > 0) {
    Serial.printf("HTTP Response Code: %d\n", httpResponseCode);
    String response = http.getString();
    Serial.println("Response:");
    Serial.println(response);
  } else {
    Serial.printf("Error on HTTP request: %s\n", http.errorToString(httpResponseCode).c_str());
  }

  // Free resources
  http.end();
}

void setup() {
  Serial.begin(115200);

  updateDisplay("UnsTable", "Initialising WiFi", 1);
  printAllWiFis();

  const char* ssid = "78463212";
  const char* password = "78463212";

  connectToWiFi(ssid, password);

  // Define the endpoint (replace "your_table" with your actual Supabase table name)
  const char* endpoint = "/desks/ee:62:5b:b8:73:1d/state";

  // Make the GET request
  APIGetRequest(endpoint);
}

void loop() {}