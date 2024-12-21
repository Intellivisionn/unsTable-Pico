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
        int melody[] = {
            523, 440, 349 // Notes: C5, A4, F4
        };
        int noteDurations[] = {
            100, 100, 150 // Durations in milliseconds
        };

        for (int i = 0; i < 3; i++) {
            tone(pin, melody[i], noteDurations[i]); // Play note
            delay(noteDurations[i] + 30);          // Short pause between notes
        }
        noTone(pin); // Stop the tone
    }
}