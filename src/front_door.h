#include <Arduino.h>

#include "pins.h"

hw_timer_t *Door_timer;
volatile bool door_timer_set = false;

volatile unsigned long door_closed_date = 0;
volatile bool door_correcly_closed_flag = false;
volatile bool door_uncorrecly_closed_flag = false;

// interrupt service routine for the door closed signal
void IRAM_ATTR door_closed()
{
    // this function will be called when the door is closed
    // log_e("door_closed()");
    digitalWrite(BUZZER_PIN, HIGH); // turn on the buzzer
    // read time
    door_closed_date = millis();
}

void IRAM_ATTR onTimer()
{
    // log_e("onTimer()");
    //  disable the timer interrupt
    timerWrite(Door_timer, 0);
    timerAlarmDisable(Door_timer);

    // if the door is closed for more than 1.8 seconds, then open it
    if ((millis() - door_closed_date > 1500) && digitalRead(I_LK_PIN) == LOW)
    {
        door_correcly_closed_flag = true;
    }
    else
    {
        // log_e("Door not closed correctly");
        door_uncorrecly_closed_flag = true;
    }
    door_timer_set = false;
}

void front_door_setup()
{
    pinMode(O_LK_PIN, OUTPUT);
    digitalWrite(O_LK_PIN, LOW);
    pinMode(I_LK_PIN, INPUT_PULLUP);
    attachInterrupt(I_LK_PIN, door_closed, FALLING);

    Door_timer = timerBegin(0, 80, true);
    timerAlarmDisable(Door_timer);
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
    if (digitalRead(I_LK_PIN) == LOW)
    {
        if (door_correcly_closed_flag)
        {
            door_correcly_closed_flag = false;
            Serial.println("Door closed");

            // test EM (please remove in the final version)
            front_door_test();
            delay(1000);
        }

        // add a timer interrupt in 2 seconds if not already done
        else if (!door_timer_set && !door_uncorrecly_closed_flag)
        {
            Serial.println("timer set");
            door_timer_set = true;
            //

            timerAttachInterrupt(Door_timer, &onTimer, true);
            timerAlarmWrite(Door_timer, 2000000, false);
            timerAlarmEnable(Door_timer);
        }
        // Serial.println("Door closed?");

        // in this function, just unlog the user, turn off leds, EMs, ...
        // todo
    }
    else if (door_uncorrecly_closed_flag)
    {
        door_uncorrecly_closed_flag = false;
        Serial.println("Door not closed correctly");
    }
}
