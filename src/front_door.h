#include <Arduino.h>

#include "pins.h"

volatile unsigned long door_closed_date = 0;
volatile unsigned long first_door_closed_date = 0;

// interrupt service routine for the door closed signal
void IRAM_ATTR door_closed()
{
    // this function will be called when the door is closed
    // log_e("door_closed()");
    digitalWrite(BUZZER_PIN, HIGH); // turn on the buzzer
    // read time
    if (door_closed_date == 0)
        fisrt_door_closed_date = millis();
    door_closed_date = millis();
}

void front_door_setup()
{
    pinMode(O_LK_PIN, OUTPUT);
    digitalWrite(O_LK_PIN, LOW);
    pinMode(I_LK_PIN, INPUT_PULLUP);
    attachInterrupt(I_LK_PIN, door_closed, FALLING);
}

void open_door()
{
    digitalWrite(O_LK_PIN, HIGH);
    delay(50);
    digitalWrite(O_LK_PIN, LOW);
    Serial.println("Door opened");
}

void front_door_test()
{
    open_door();
    digitalWrite(BUZZER_PIN, LOW);
}

void front_door_loop()
{
    if (door_closed_date != 0) // door closed ? (recently detected)
    {
        if (digitalRead(I_LK_PIN) == LOW) // detected door closed
        {

            if (millis() - door_closed_date > 2000)
            {
                // door closed for more than 2s
                door_closed_date = 0;
                // door closed correctly
                log_e("Door closed correctly for more than 2s");
                //TODO : save which user has closed the door correctly, and log it
                open_door();
            }

            // in this function, just unlog the user, turn off leds, EMs, ...
            // todo
        }
        if (millis() - first_door_closed_date > 10000)
        {
            // door unclosed for more than 10s

            // door closed uncorrectly
            log_e("Door closed uncorrectly for more than 10s");
            door_closed_date = 0;

            // TODO : save which user has not closed the door correctly, and log it

        }
    }
