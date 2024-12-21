#include "display.h"
#include <qrcode.h>

// Constructor
Display::Display(int width, int height) : display(width, height, &Wire, -1) {
    currentDisplayState = NONE;  // Initialize the state as 'NONE'
    currentMessage[0] = '\0';  // Initialize the current message as empty
}

// Initialize the display
void Display::begin() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.display();
}

// Clear the display (except for the timer)
void Display::clear() {
    display.clearDisplay();
    currentDisplayState = NONE;  // Reset state to NONE after clearing
    currentMessage[0] = '\0';  // Clear the current message
    display.display();
}

// Display text on the screen
void Display::showText(const char* message) {
    if (currentDisplayState != TEXT) {
        display.clearDisplay();  // Clear only when state changes
        currentDisplayState = TEXT;  // Set the state to TEXT
    }

    strncpy(currentMessage, message, sizeof(currentMessage) - 1); // Save the message
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print(message);
    display.display();
}
// Display a QR code on the screen
void Display::showQRCode(const char* content) {
    if (currentDisplayState != QR_CODE && timerRunning == false) {
        display.clearDisplay();  // Clear only when state changes
        currentDisplayState = QR_CODE;  // Set the state to QR_CODE

        QRCode qrcode;
        uint8_t qrcodeData[qrcode_getBufferSize(3)];
        qrcode_initText(&qrcode, qrcodeData, 3, ECC_LOW, content);

        int offset_x = (display.width() - qrcode.size * 2) / 2;
        int offset_y = (display.height() - qrcode.size * 2) / 2;

        // Display the QR code
        for (uint8_t y = 0; y < qrcode.size; y++) {
            for (uint8_t x = 0; x < qrcode.size; x++) {
                if (qrcode_getModule(&qrcode, x, y)) {
                    display.fillRect(offset_x + x * 2, offset_y + y * 2, 2, 2, SSD1306_WHITE);
                }
            }
        }

        // Display the first 4 characters of content on the left side, centered vertically
        char firstFourChars[5];
        strncpy(firstFourChars, content, 4);
        firstFourChars[4] = '\0';

        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, (display.height() - 16) / 2);  // 16 is the approximate height of two lines of text
        display.print(firstFourChars);

        display.display();
    }
}

// Start the timer
void Display::startTimer(unsigned long offsetMillis = 0) {
    timerRunning = true;
    startTime = millis() - offsetMillis;
}

// Stop the timer (clear when logout or disconnect)
void Display::stopTimer() {
    timerRunning = false;
}

// Update the timer display
void Display::updateTimer() {
    if (timerRunning) {
        unsigned long elapsed = millis() - startTime;
        int seconds = (elapsed / 1000) % 60;
        int minutes = (elapsed / 1000) / 60;

        char timeString[16];
        snprintf(timeString, sizeof(timeString), "Time: %02d:%02d", minutes, seconds);

        // Save current state of the screen
        String previousMessage = currentMessage;

        // Clear only the bottom section for the timer
        display.fillRect(0, display.height() - 10, display.width(), 10, SSD1306_BLACK);  // Clear the bottom part

        // Set the text size for the timer
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);  // Set text color

        // Set the cursor to the bottom-left of the screen for the timer
        display.setCursor(0, display.height() - 10);  // Place at the bottom
        display.print(timeString);

        // Redraw the saved message at the top of the screen
        if (currentDisplayState == TEXT) {
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 0);
            display.print(previousMessage);
        } else if (currentDisplayState == QR_CODE) {
            // Handle redrawing the QR code if it was displayed
            showQRCode(currentMessage); // Since `showQRCode` already updates the screen, it will work here.
        }

        display.display(); // Update the display
    }
}

// Animations (startup animations, etc.)
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