#include "button.h"

Button::Button(uint8_t pin, void (*clickCallback)(), void (*doubleClickCallback)()) 
    : button(pin, true) { // true for internal pull-up
    button.attachClick(clickCallback);
    button.attachDoubleClick(doubleClickCallback);
}

void Button::tick() {
    button.tick();
}
