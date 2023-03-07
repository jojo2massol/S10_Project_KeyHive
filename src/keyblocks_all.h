#pragma once

#include "keyblock.h"
#include "user.h"

#define nKeyblocks 2 // number of keyblocks

Keyblock keyblocks[nKeyblocks];
#define KBlk keyblocks[0] // remove in the final version
volatile bool EM_on_flag = false;
volatile unsigned long EM_on_time = 0;

volatile bool keyblocks_off = false;
volatile bool keyblocks_interrupt_flag = true;

void IRAM_ATTR keyblock_interrupt()
{

    keyblocks_interrupt_flag = true;
}

void keyblock_loop()
{
    // check if user logged in
    if (user.logged_in)
    {
        if (keyblocks_interrupt_flag)
        {
            keyblocks_interrupt_flag = false;
            // Serial.print("int|");
            //  read all keyblocks, and check if any have a limit switch pressed or a push button pressed

            for (int i = 0; i < nKeyblocks; i++)
            {
                // read last state of the button
                bool last_bt_state = keyblocks[i].getPushButton(false);
                if (keyblocks[i].getPushButton(true) != last_bt_state)
                {
                    // Serial.println("Button state changed: " + String(keyblocks[i].getPushButton()));
                    EM_on_time = millis();
                    // reset blue light
                    keyblocks[i].setLED(false, false, true, true); // blue
                }

                /*
                 if key Locking :
                 - check if limit switch pressed
                   if so:
                      - unpower the electromagnet,
                      - check if any keyblocks have EM powered with getEM(),
                        if not so:
                          - set EM_on_flag to false
                 - else if button pressed or just released, set EM_on_time to millis()
                */

                if (keyblocks[i].key_state == KEY_LOCKING)
                {
                    if (keyblocks[i].getLimitSwitch(true))
                    {
                        if (keyblocks[i].getEM())
                        {
                            keyblocks[i].setEM(false);
                            keyblocks[i].setLED(false, true, false, true); // green
                            keyblocks[i].key_state = KEY_LOCKED;
                            Serial.println("Key " + String(i) + " locked after KEY_LOCKING");
                            for (int j = 0; j < nKeyblocks; j++)
                            {
                                if (keyblocks[j].getEM())
                                {
                                    break;
                                }
                                else if (j == nKeyblocks - 1)
                                { // if no EM is powered, set EM_on_flag to false
                                    EM_on_flag = false;
                                    EM_on_time = 0;
                                }
                            }
                        }
                    }
                }

                /*
                 if key released :
                 - check if push button pressed
                 - check if the user is allowed to set the key,
                   if so, set the EM, and set the EM_on_flag to true.
                */

                else if (keyblocks[i].key_state == KEY_RELEASED)
                {
                    if (keyblocks[i].getPushButton(true))
                    {
                        // TODO: check if user is allowed to free the key

                        Serial.println("Key " + String(i) + " locking after KEY_RELEASED");
                        keyblocks[i].key_state = KEY_LOCKING;
                        keyblocks[i].setEM(true, true);
                        keyblocks[i].setLED(false, false, true, true); // magenta
                        EM_on_flag = true;
                        EM_on_time = millis();
                    }
                }
                /*
                 if key locked :
                 - check if push button pressed
                 - check if the user is allowed to free the key,
                   if so, set the EM, and set the EM_on_flag to true.
                */

                else if (keyblocks[i].key_state == KEY_LOCKED)
                {
                    if (keyblocks[i].getPushButton(true))
                    {
                        // TODO: check if user is allowed to free the key

                        Serial.println("Key " + String(i) + " releasing after KEY_LOCKED");
                        keyblocks[i].setEM(true, true);
                        keyblocks[i].setLED(false, true, true, true); // cyan
                        keyblocks[i].key_state = KEY_RELEASING;
                        EM_on_flag = true;
                        EM_on_time = millis();
                    }
                }
            }
        }

        // check if timout has been reached
        if (EM_on_flag)
        {
            if (millis() - EM_on_time > 4000)
            {
                Serial.println("Timeout reached, turning off EM, and saving 'released' state");
                // if so, turn off the EM
                for (int i = 0; i < nKeyblocks; i++)
                {
                    if (keyblocks[i].getEM())
                    {
                        keyblocks[i].setEM(false, true);
                        keyblocks[i].setLED(true, true, false, true); // yellow
                        keyblocks[i].key_state = KEY_RELEASED;
                    }
                }
                EM_on_flag = false;
                EM_on_time = 0;
            }
            else if (millis() - EM_on_time > 2000)
            {
                for (int i = 0; i < nKeyblocks; i++)
                {
                    if (keyblocks[i].getEM())
                    {
                        keyblocks[i].setLED(true, false, true, true); // magenta
                    }
                }
            }
        }

        if (keyblocks_off)
        {
            keyblocks_off = false;
            Serial.println("keyblocks_off :");
            Serial.println(keyblocks_off);
            Serial.println("User logged in, turning on keyblocks");
            // read permission of the user for each key
            for (int i = 0; i < nKeyblocks; i++)
            {
                // TODO : read permission of the user for each key
                // if permission is granted, allowed keys can be released.

                /*
                permission not granted:
                - key is red
                if permission granted:
                - locked key is green
                - released key is yellow
                - locking key is magenta
                - releasing key is cyan
                */
                if (check_card(uid, uidLength))
                {
                    if (keyblocks[i].key_state == KEY_LOCKED)
                    {
                        keyblocks[i].setLED(false, true, false, true); // green
                    }
                    else if (keyblocks[i].key_state == KEY_RELEASED)
                    {
                        keyblocks[i].setLED(true, true, false, true); // yellow
                    }
                    else if (keyblocks[i].key_state == KEY_LOCKING)
                    {
                        keyblocks[i].setLED(true, false, true, true); // magenta
                    }
                    else if (keyblocks[i].key_state == KEY_RELEASING)
                    {
                        keyblocks[i].setLED(false, true, true, true); // cyan
                    }
                }
                else
                {
                    keyblocks[i].setLED(true, false, false, true); // red
                }
            }
        }
    }
    else if (keyblocks_off == false) // if user not logged in, turn off all EMs and leds
    {                                // close EMs
        Serial.println("User not logged in, turning off keyblocks");
        for (int i = 0; i < nKeyblocks; i++)
        {
            if ((keyblocks[i].key_state == KEY_LOCKING) || (keyblocks[i].key_state == KEY_RELEASING))
            { // unknown state, set to released
                keyblocks[i].key_state = KEY_RELEASED;
                log_e("Key uncorrecly set: %d state changed to KEY_RELEASED", i);
            }

            // close all EMs, and turn all leds off
            keyblocks[i].setEM(false);
            keyblocks[i].setLED(false, false, false, true);
        }
        EM_on_flag = false;
        EM_on_time = 0;

        keyblocks_off = true;
    }
}
