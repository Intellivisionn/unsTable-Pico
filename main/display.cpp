#include "display.h"

Display::Display(uint8_t width, uint8_t height)
    : display(width, height, &Wire, -1) {}

void Display::begin() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 initialization failed!");
        while (true);
    }
    clear();
}

void Display::clear() {
    display.clearDisplay();
    display.display();
}

void Display::showText(const char* text) {
    clear();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(text);
    display.display();
}

void Display::showQRCode(const char* content) {
    clear();
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    qrcode_initText(&qrcode, qrcodeData, 3, ECC_LOW, content);

    int offset_x = (128 - qrcode.size * 2) / 2;
    int offset_y = (64 - qrcode.size * 2) / 2;

    for (uint8_t y = 0; y < qrcode.size; y++) {
        for (uint8_t x = 0; x < qrcode.size; x++) {
            if (qrcode_getModule(&qrcode, x, y)) {
                display.fillRect(offset_x + x * 2, offset_y + y * 2, 2, 2, SSD1306_WHITE);
            }
        }
    }
    display.display();
}
