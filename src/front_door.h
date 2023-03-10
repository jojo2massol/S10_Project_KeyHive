#pragma once

#include <Arduino.h>

#include "pins.h"
#include "user.h"
#include "NFCread.h"
#include "esp32-hal-log.h"

volatile unsigned long door_change_date = 0;
volatile unsigned long last_door_change_date = 0;

#define DOOR_CLOSED 0
#define DOOR_OPEN 1
#define DOOR_OPENING 2
#define DOOR_SHOULD_BE_CLOSED 3

volatile uint8_t door_state = DOOR_CLOSED;
volatile bool door_changed = true;

void open_door()
{
    door_state = DOOR_OPEN;
    door_changed = true;
    digitalWrite(O_LK_PIN, HIGH);
    // wait until the door is open, or 5 seconds
    volatile unsigned long opentime = millis();
    while (digitalRead(I_LK_PIN) == LOW && millis() - opentime < 5000)
    {
    }
    digitalWrite(O_LK_PIN, LOW);
    if (digitalRead(I_LK_PIN) == LOW)
    {
        // the door is still closed, issue
        log_e("Door is still closed after 5 seconds");
    }
    else
    {
        Serial.println("Door opened in " + String(millis() - opentime) + " ms");
    }
}

// interrupt service routine for the door closed signal
void IRAM_ATTR door_int()
{
    door_change_date = millis();
    door_changed = true;
}

void front_door_setup()
{
    pinMode(O_LK_PIN, OUTPUT);
    digitalWrite(O_LK_PIN, LOW);
    pinMode(I_LK_PIN, INPUT_PULLUP);
    attachInterrupt(I_LK_PIN, door_int, CHANGE);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);
    pinMode(LED_R, OUTPUT);
    digitalWrite(LED_G, HIGH);
    digitalWrite(LED_B, HIGH);
    digitalWrite(LED_R, HIGH);

    door_state = digitalRead(I_LK_PIN) == LOW ? DOOR_CLOSED : DOOR_OPEN;
    door_changed = true;
}

void front_door_loop()
{
    if ((door_state == DOOR_CLOSED) || (door_state == DOOR_SHOULD_BE_CLOSED))
    { // NFC read
        uint8_t uid[User_max_length];
        uint8_t uidLength = User_max_length;
        if (NFC_read(uid, &uidLength))
        {
            user.logIn(uid, &uidLength);
            // check if user is allowed to open the door
            if (user.canOpenDoor())
            {
                Serial.println("User is allowed to open the door");
                Serial.print("UID: ");
                for (int i = 0; i < user.uidLength; i++)
                {
                    Serial.print(user.uid[i], HEX);
                }
                Serial.println();
                // open the door
                door_state = DOOR_OPENING;
                door_changed = true;
            }
            else
            {
                // buzzer
                digitalWrite(BUZZER_PIN, HIGH);
                digitalWrite(LED_G, LOW);
                delay(200);
                digitalWrite(BUZZER_PIN, LOW);
                digitalWrite(LED_G, HIGH);
                delay(50);
            }
        }
    }

    if (DOOR_SHOULD_BE_CLOSED == door_state)
    {
        if (millis() - door_change_date > 2000)
        {
            if (digitalRead(I_LK_PIN) == LOW)
            {
                door_state = DOOR_CLOSED;
                door_changed = true;
                if (user.logged_in)
                {
                    Serial.println("User logged out. UID :");
                    Serial.print("UID: ");
                    for (int i = 0; i < user.uidLength; i++)
                    {
                        Serial.print(user.uid[i], HEX);
                    }
                }
                Serial.println();

                Serial.println("keyblocks_off :");
                Serial.println(keyblocks_off);
            }
            else
            {
                // buzzer on
                digitalWrite(BUZZER_PIN, HIGH);
                log_e("User uncorrectly closed the door");
                Serial.print("UID: ");
                for (int i = 0; i < user.uidLength; i++)
                {
                    Serial.print(user.uid[i], HEX);
                }
                Serial.println();
            }
        }
    }
    else
    {
        // buzz off
        //log_e("%i", door_state);
        //digitalWrite(BUZZER_PIN, LOW);
    }

    if (door_changed)
    {
        door_changed = false;
        // Serial.print("Changed :" + String(digitalRead(I_LK_PIN)));
        if (digitalRead(I_LK_PIN) == LOW)
        { // door just closed
            switch (door_state)
            {
            case DOOR_CLOSED:
                // the door is closed for more than 2 seconds, everything is fine
                Serial.println("door is closed for more than 2 seconds");
                user.logOut();
                // red led
                digitalWrite(LED_B, HIGH);
                digitalWrite(LED_G, HIGH);
                digitalWrite(LED_R, LOW);
                digitalWrite(BUZZER_PIN, LOW);
                break;
            case DOOR_OPENING:
                Serial.println("door will open soon");
                // the door should be open soon
                // cyan led
                digitalWrite(LED_B, LOW);
                digitalWrite(LED_G, LOW);
                digitalWrite(LED_R, HIGH);
                open_door();
                break;
            case DOOR_OPEN:
                // door probably just closed
                Serial.println("door is considered open, but is closed. Closing it.");
                // blue led
                digitalWrite(LED_B, LOW);
                digitalWrite(LED_G, HIGH);
                digitalWrite(LED_R, HIGH);
                door_state = DOOR_SHOULD_BE_CLOSED;
                door_changed = true;
                door_change_date = millis();
                break;
            case DOOR_SHOULD_BE_CLOSED:
                // door is closed, and stay shoud be until 2 seconds
                if (millis() - door_change_date > 2000)
                {
                    door_state = DOOR_CLOSED;
                    door_changed = true;
                    if (user.logged_in)
                    {
                        Serial.println("User logged out. UID :");
                        for (int i = 0; i < user.uidLength; i++)
                        {
                            Serial.print(user.uid[i], HEX);
                        }
                        Serial.println();
                    }
                    Serial.println("door just closed after 2 seconds");
                }
                else
                {
                    Serial.println("door is closed, and will be considered as closed in 2 seconds");
                }
            }
        }
        else
        { // door opened
            switch (door_state)
            {

            case DOOR_OPENING:
                // the door is opening, everything is fine
                Serial.println("door is opening");
                // cyan led
                digitalWrite(LED_B, LOW);
                digitalWrite(LED_G, LOW);
                digitalWrite(LED_R, HIGH);
                open_door();
                break;
            case DOOR_OPEN:
                // the door is open, everything is fine
                Serial.println("door is open");
                // green led
                digitalWrite(LED_B, HIGH);
                digitalWrite(LED_G, LOW);
                digitalWrite(LED_R, HIGH);
                break;
            case DOOR_SHOULD_BE_CLOSED:
                // the door is open, but should be closed, alarm !!!
                Serial.println("door is open, but should be closed, alarm !!!");
                // magenta led
                digitalWrite(LED_B, LOW);
                digitalWrite(LED_G, HIGH);
                digitalWrite(LED_R, LOW);
                break;
            case DOOR_CLOSED:
                // the door is closed, but should be open, alarm !!!
                Serial.println("door is open, but should be considered as closed, alarm !!!");
                door_state = DOOR_SHOULD_BE_CLOSED;
                // magenta led
                digitalWrite(LED_B, LOW);
                digitalWrite(LED_G, HIGH);
                digitalWrite(LED_R, LOW);
                digitalWrite(BUZZER_PIN, HIGH);
                break;
            }
        }
    }
}
