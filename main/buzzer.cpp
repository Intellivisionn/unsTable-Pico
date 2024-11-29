#include "buzzer.h"

bool notificationsEnabled = true;

Buzzer::Buzzer(uint8_t buzzerPin) {
    pin = buzzerPin;
    pinMode(pin, OUTPUT);
}

void Buzzer::setEnabled(bool enabled) {
    notificationsEnabled = enabled;
}

void Buzzer::playNotification() {
    if (notificationsEnabled) {
            digitalWrite(pin, HIGH);
            delay(200);
            digitalWrite(pin, LOW);
            delay(200);
            digitalWrite(pin, HIGH);
            delay(200);
            digitalWrite(pin, LOW);
    }
}
