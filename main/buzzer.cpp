#include "buzzer.h"
#include <Arduino.h>

int buzzerPin;

void setupBuzzer(int pin) {
  buzzerPin = pin;
  pinMode(buzzerPin, OUTPUT);
}

// Generate a tone on the given pin
void tone(uint8_t pin, unsigned int frequency, unsigned long duration) {
    unsigned long halfPeriod = 1000000L / (frequency * 2);
    unsigned long cycles = frequency * duration / 1000;

    for (unsigned long i = 0; i < cycles; i++) {
        digitalWrite(pin, HIGH);
        delayMicroseconds(halfPeriod);
        digitalWrite(pin, LOW);
        delayMicroseconds(halfPeriod);
    }
}

// Stop the tone on the given pin
void noTone(uint8_t pin) {
    digitalWrite(pin, LOW); // Ensure the pin is low
}

// Function for notification sound
void notificationSound() {
    tone(BUZZER_PIN, 1000, 200); // Play 1 kHz tone for 200 ms
    delay(300); // Wait for 300 ms
    tone(BUZZER_PIN, 1500, 200); // Play 1.5 kHz tone for 200 ms
    delay(300); // Wait for 300 ms
    noTone(BUZZER_PIN);         // Stop the tone
}