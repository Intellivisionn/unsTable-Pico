#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <qrcode.h>

class Display {
private:
    Adafruit_SSD1306 display;

public:
    Display(uint8_t width, uint8_t height);
    void begin();
    void clear();
    void showText(const char* text);
    void showQRCode(const char* content);
};

#endif
