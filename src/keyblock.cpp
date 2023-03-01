#include "keyblock.h"

uint8_t Keyblock::set(uint8_t data)
{
    state = data;
    Wire.beginTransmission(address);
    Wire.write(state);
    Wire.endTransmission();
    return state;
}
Keyblock::Keyblock()
: address(0x00)
{}

Keyblock::Keyblock(uint8_t address)
: address(address)
{
    set(0xFF);
    // Remarques :
    //    - mettre à "1" une E/S du PCF8574 revient à permettre d'utiliser cette broche en ENTRÉE
    //    - on aura tout de même une tension +Vcc présente sur cette broche, si celle-ci n'est reliée à rien
    //    - cet état haut est dit "faible", en ce sens où on peut l'amener à la masse sans créer de court-circuit
    //      (c'est l'équivalent d'une pull-up, qui serait activée sur cette broche, si vous préférez)
}

uint8_t Keyblock::read()
{
    Wire.requestFrom(address, (uint8_t)1);
    for (int i = 0; i < 5; i++)
    {
        if (Wire.available())
        {
            state = Wire.read();
            break;
        }
        // display file name and line number error
        Serial.println("Error: device " + String(address) + "unavailable, try=" + String(i + 1) + "/5 : " + String(__FILE__) + " line " + String(__LINE__));
        delay(50);
    }
    return state;
}


void Keyblock::begin()
{
    Wire.begin();
}

bool Keyblock::getpin(uint8_t pin)
{
    return (state & (1 << pin)) != 0;
}

void Keyblock::setpin(uint8_t pin, bool state, bool apply)
{
    if (state)
    {
        this->state |= (1 << pin);
    }
    else
    {
        this->state &= ~(1 << pin);
    }
    if (apply)
    {
        set(this->state);
    }
}

void Keyblock::setEM(bool state, bool apply)
{
    setpin(ELECTROMAGNET_PIN, state, apply);
}

void Keyblock::setaddress(uint8_t address)
{
    this->address = address;
}

void Keyblock::setLED(bool red, bool green, bool blue, bool apply)
{
    // do all in one call to variable state
    uint8_t LEDS = red << LED_R_PIN | blue << LED_B_PIN | green << LED_G_PIN;
    state = (state & B11110001) | LEDS;
}

bool Keyblock::getLimitSwitch(bool update)
{
    if (update)
    {
        read();
    }
    return getpin(LIMIT_SWITCH_PIN);
}

bool Keyblock::getPushButton(bool update)
{
    if (update)
    {
        read();
    }
    return getpin(PUSH_BUTTON_PIN);
}

void Keyblock::test_keyblock()
{
    // test de la broche 0 (électroaimant)
    setEM(true);
    delay(1000);
    setEM(false);
    delay(1000);
    // test blanc (rouge + vert + bleu)
    setLED(true, true, true);
    // test de la broche 1 (LED rouge)
    setLED(true, false, false);
    delay(1000);
    // test de la broche 2 (LED verte)
    setLED(false, true, false);
    delay(1000);
    // test de la broche 3 (LED bleue)
    setLED(false, false, true);
    delay(1000);
    // test jaune (rouge + vert)
    setLED(true, true, false);
    delay(1000);
    // test cyan (vert + bleu)
    setLED(false, true, true);
    delay(1000);
    // test magenta (rouge + bleu)
    setLED(true, false, true);
    delay(1000);
}
