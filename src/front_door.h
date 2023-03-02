#include <Arduino.h>

#include "pins.h"

// interrupt service routine for the door closed signal
void IRAM_ATTR door_closed()
{                                   // this function will be called when the door is closed
    digitalWrite(BUZZER_PIN, HIGH); // turn on the buzzer
}

void front_door_setup()
{
    pinMode(O_LK_PIN, OUTPUT);
    digitalWrite(O_LK_PIN, LOW);
    pinMode(I_LK_PIN, INPUT_PULLUP);
    attachInterrupt(I_LK_PIN, door_closed, FALLING);
}

void front_door_test()
{
    delay(2000);
    digitalWrite(O_LK_PIN, HIGH);
    delay(50);
    digitalWrite(O_LK_PIN, LOW);
    Serial.println("Door opened");
    digitalWrite(BUZZER_PIN, LOW);
}

void front_door_loop()
{
    if (digitalRead(I_LK_PIN) == LOW)
    {
        Serial.println("Door closed");

        // in this function, just unlog the user, turn off leds, EMs, ...
        // todo

        // test EM (please remove in the final version)
        front_door_test();
    }
}

