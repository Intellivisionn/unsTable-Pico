#include "display.h"
#include <qrcode.h>

// Constructor
Display::Display(int width, int height) : display(width, height, &Wire, -1) {}

// Initialize the display
void Display::begin() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.display();
}

// Clear the display
void Display::clear() {
    display.clearDisplay();
    display.display();
}

// Display text
void Display::showText(const char* message) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print(message);
    display.display();
}

// Display a QR code
void Display::showQRCode(const char* content) {
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    qrcode_initText(&qrcode, qrcodeData, 3, ECC_LOW, content);

    display.clearDisplay();

    int offset_x = (display.width() - qrcode.size * 2) / 2;
    int offset_y = (display.height() - qrcode.size * 2) / 2;

    for (uint8_t y = 0; y < qrcode.size; y++) {
        for (uint8_t x = 0; x < qrcode.size; x++) {
            if (qrcode_getModule(&qrcode, x, y)) {
                display.fillRect(offset_x + x * 2, offset_y + y * 2, 2, 2, SSD1306_WHITE);
            }
        }
    }

    display.display();
}

// Scrolling Effect
void Display::showStartupScrolling(const char* message) {
    int messageLength = strlen(message);
    int scrollWidth = display.width() / 6; // Text width estimation
    char buffer[scrollWidth + 1];

    for (int i = 0; i < messageLength + 1; i++) {
        memset(buffer, ' ', scrollWidth); // Fill buffer with spaces
        buffer[scrollWidth] = '\0';

        for (int j = 0; j < scrollWidth && (i + j) < messageLength; j++) {
            buffer[j] = message[i + j];
        }

        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.print(buffer);
        display.display();
        delay(200);
    }
}

// Typing Animation Effect
void Display::showStartupTyping(const char* line1, const char* line2) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    for (size_t i = 0; i < strlen(line1); i++) {
        display.print(line1[i]);
        display.display();
        delay(150); // Typing speed
    }

    display.setCursor(0, 10);

    for (size_t i = 0; i < strlen(line2); i++) {
        display.print(line2[i]);
        display.display();
        delay(150); // Typing speed
    }
}

// Combined Animation Effect
void Display::showStartupCool(const char* line1, const char* line2) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Typing effect for line 1
    display.setCursor(0, 0);
    for (size_t i = 0; i < strlen(line1); i++) {
        display.print(line1[i]);
        display.display();
        delay(150);
    }

    // Scrolling effect for line 2
    for (int offset = 0; offset < display.width(); offset += 2) {
        display.clearDisplay();
        display.setCursor(display.width() - offset, 16);
        display.print(line2);
        display.display();
        delay(50); // Scrolling speed
    }
}

unsigned long startTime = 0;
bool timerRunning = false;

// Start the timer
void Display::startTimer() {
    timerRunning = true;
    startTime = millis();
}

// Stop the timer
void Display::stopTimer() {
    timerRunning = false;
    clear(); // Optionally clear the display when the timer stops
}

// Update the timer display
void Display::updateTimer() {
    if (timerRunning) {
        unsigned long elapsed = millis() - startTime;
        int seconds = (elapsed / 1000) % 60;
        int minutes = (elapsed / 1000) / 60;

        char timeString[16];
        snprintf(timeString, sizeof(timeString), "Time: %02d:%02d", minutes, seconds);

        display.clearDisplay();   // Clear the display
        display.setTextSize(1);  // Set text size
        display.setCursor(0, 0); // Set cursor position
        display.print(timeString);
        display.display();       // Render the changes
    }
}