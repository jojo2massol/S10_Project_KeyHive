#include <WiFi.h>
#include "server_mode.h"
#include "keyblock.h"
#include "scanner_I2C.h"
#include "pins.h"
#include "sdcard.h"



esp_sleep_wakeup_cause_t wakeup_reason;
const uint8_t nKeyblocks = 1;   // will be 9 in the final version
Keyblock keyblocks[nKeyblocks];
#define KBlk keyblocks[0]       // remove in the final version
/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason()
{

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void IRAM_ATTR door_closed()
{ // this function will be called when the door is closed
  digitalWrite(BUZZER_PIN, HIGH); // turn on the buzzer
}
void setup()
{
  Serial.begin(921600); // Start serial, to output debug data
  while (!Serial)
    ; // Wait for user to open terminal
  Serial.println(F("Serial connected"));

  // Print the wakeup reason for ESP32
  print_wakeup_reason();

  // Set up the keyblocks
  keyblocks[0].setaddress(0x20);

  // buzzer as output
  pinMode(BUZZER_PIN, OUTPUT);
  // Lock
  pinMode(O_LK_PIN, OUTPUT);
  digitalWrite(O_LK_PIN, LOW);
  pinMode(I_LK_PIN, INPUT_PULLUP);
  attachInterrupt(I_LK_PIN, door_closed, FALLING);

  // server_mode();

  // scanner_setup();
  // SDcard_test();
}

void loop()
{
  // KBlk.test_keyblock();
  // scanner_loop();

  /*
  // buzzer test
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
  delay(50);*/

  // EM test
  if (digitalRead(I_LK_PIN) == LOW)
  {
    Serial.println("Door closed");

    // in this function, just unlog the user, turn off leds, EMs, ...
    //todo


    //test EM (please remove in the final version)
    delay(2000);
    digitalWrite(O_LK_PIN, HIGH);
    delay(50);
    digitalWrite(O_LK_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
  }
}
