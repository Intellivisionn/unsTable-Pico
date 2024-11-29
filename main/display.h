#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Display {
private:
    Adafruit_SSD1306 display;

public:
    Display(int width, int height);
    void begin();
    void clear();
    void showText(const char* message);
    void showQRCode(const char* content);

    // New animations
    void showStartupScrolling(const char* message);
    void showStartupTyping(const char* line1, const char* line2);
    void showStartupCool(const char* line1, const char* line2);
};

#endif