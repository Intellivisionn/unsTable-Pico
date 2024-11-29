#ifndef BUTTON_H
#define BUTTON_H

#include <OneButton.h>

class Button {
private:
    OneButton button;

public:
    Button(uint8_t pin, void (*clickCallback)(), void (*doubleClickCallback)());
    void tick();
};

#endif