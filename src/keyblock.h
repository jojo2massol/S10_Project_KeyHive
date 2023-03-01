/* Class for the PCF8574 on the keyblock,
communicates with the PCF8574 via I2C.
*/

#include <Wire.h>
#include "arduino.h"
#define ELECTROMAGNET_PIN 0 // output
#define LED_R_PIN 1         // output
#define LED_B_PIN 2         // output
#define LED_G_PIN 3         // output
#define LIMIT_SWITCH_PIN 6  // input
#define PUSH_BUTTON_PIN 7   // input



class Keyblock  
{
private:
    uint8_t address;
    uint8_t state;
    uint8_t set(uint8_t data);
public:
    Keyblock();
    Keyblock(uint8_t address);
    void begin();                           // execute only once for all keyblocks (Wire.begin())
    void setaddress(uint8_t address);       // set the address of the keyblock
    uint8_t read();                         // read the state of the keyblock, that can be used to get the state of the pins individually then
    bool getpin(uint8_t pin);            // 0-7                  can be private
    void setpin(uint8_t pin, bool state, bool apply=false);   // 0-7 
    
    void setEM(bool state, bool apply=false);
    void setLED(bool red, bool green, bool blue, bool apply=false);
    bool getLimitSwitch(bool update=false);
    bool getPushButton(bool update=false);
};