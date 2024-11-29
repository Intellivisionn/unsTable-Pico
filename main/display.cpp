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

// Render pending changes to the display
void Display::clearAndDisplay() {
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