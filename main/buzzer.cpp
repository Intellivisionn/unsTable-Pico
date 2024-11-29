#include "buzzer.h"

Buzzer::Buzzer(uint8_t buzzerPin) {
    pin = buzzerPin;
    pinMode(pin, OUTPUT);
}

void Buzzer::playNotification() {
    digitalWrite(pin, HIGH);
    delay(200);
    digitalWrite(pin, LOW);
    delay(200);
    digitalWrite(pin, HIGH);
    delay(200);
    digitalWrite(pin, LOW);
}
