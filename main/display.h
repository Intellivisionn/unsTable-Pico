#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_SSD1306.h>
#include <Wire.h>

// Define the display width and height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Display state definitions
#define NONE 0
#define TEXT 1
#define QR_CODE 2

class Display {
public:
    // Constructor
    Display(int width, int height);

    // Initialize the display
    void begin();

    // Clear the display (except for the timer)
    void clear();

    // Show a message on the display
    void showText(const char* message);

    // Show a QR code on the display
    void showQRCode(const char* content);

    // Start the timer
    void startTimer();

    // Stop the timer (clear when logout or disconnect)
    void stopTimer();

    // Update the timer display
    void updateTimer();

    // Animations (startup animations, etc.)
    void showStartupScrolling(const char* message);
    void showStartupTyping(const char* line1, const char* line2);
    void showStartupCool(const char* line1, const char* line2);

private:
    Adafruit_SSD1306 display;
    int currentDisplayState;  // Keeps track of the current display state (text, QR code, etc.)
    char currentMessage[128]; // Stores the current message to preserve content during timer updates
    unsigned long startTime;  // Start time for the timer
    bool timerRunning;        // Whether the timer is running or not
};

#endif // DISPLAY_H