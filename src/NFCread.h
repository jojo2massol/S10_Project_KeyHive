#include <Wire.h>
#include <Adafruit_PN532.h> //NFC

// i2c NFC not SPI
Adafruit_PN532 nfc(2, 3); // not sure of 2 and 3

void NFC_setup()
{
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata)
    {
        Serial.print("Didn't find PN53x board");
        while (1)
            ; // halt
    }
    // Got ok data, print it out!
    Serial.print("Found chip PN5");
    Serial.println((versiondata >> 24) & 0xFF, HEX);
    Serial.print("Firmware ver. ");
    Serial.print((versiondata >> 16) & 0xFF, DEC);
    Serial.print('.');
    Serial.println((versiondata >> 8) & 0xFF, DEC);

    // configure board to get the serial number of a card
    nfc.SAMConfig();
}

void NFC_loop()
{
    // get serial number of a card
    uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t uidLength;

    // wait for an ISO14443A type cards (Mifare, etc.)
    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength))
    {
        Serial.println("Found a card!");
        Serial.print("UID Length: ");
        Serial.print(uidLength, DEC);
        Serial.println(" bytes");
        Serial.print("UID Value: ");
        nfc.PrintHex(uid, uidLength);
        Serial.println("");
    }
    else
    {
        Serial.println("No card found");
    }
}
