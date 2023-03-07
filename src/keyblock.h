#pragma once

/* Class for the PCF8574 on the keyblock,
communicates with the PCF8574 via I2C.
*/

#include <Wire.h>
#include "arduino.h"
#include "keyblock_csts.h"

class Keyblock
{
private:
    uint8_t address;
    uint8_t state;

public:
    uint8_t key_state = KEY_RELEASED;
    uint8_t set(uint8_t data);
    Keyblock();
    Keyblock(uint8_t address);
    void begin();                                             // execute only once for all keyblocks (Wire.begin())
    void setaddress(uint8_t address);                         // set the address of the keyblock
    uint8_t read();                                           // read the state of the keyblock, that can be used to get the state of the pins individually then
    bool getpin(uint8_t pin);                                 // 0-7                  can be private
    void setpin(uint8_t pin, bool state, bool apply = false); // 0-7

    void setEM(bool state, bool apply = false);
    bool getEM();
    void setLED(bool red, bool green, bool blue, bool apply = false);
    bool getLimitSwitch(bool update = false);
    bool getPushButton(bool update = false);
    void test_keyblock();
};