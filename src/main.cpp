#include <WiFi.h>
#include "server_mode.h"
#include "keyblock.h"


esp_sleep_wakeup_cause_t wakeup_reason;
const uint8_t nKeyblocks = 1; // will be 9 in the final version
Keyblock keyblocks[nKeyblocks];
#define KBlk keyblocks[0]
/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason(){

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void setup()
{
  Serial.begin(921600); // Start serial, to output debug data
  while (!Serial)
    ; // Wait for user to open terminal
  Serial.println(F("Serial connected"));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  // Set up the keyblocks
  keyblocks[0].setaddress(0x20);

  server_mode();
}




void loop()
{
  KBlk.test_keyblock();
}
