#include <WiFi.h>

#include "server_mode.h"
#include "keyblocks_all.h"
#include "scanner_I2C.h"
#include "pins.h"
#include "sdcard.h"
#include "front_door.h"
#include "NFCread.h"
#include "user.h"

esp_sleep_wakeup_cause_t wakeup_reason;

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason()
{

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    break;
  }
}

void setup()
{
  // Set up the keyblocks
  KBlk.begin();
  keyblocks[0].setaddress(0x27);

  Serial.begin(921600); // Start serial, to output debug data
  while (!Serial)
    ; // Wait for user to open terminal
  Serial.println(F("Serial connected"));

  // Print the wakeup reason for ESP32
  print_wakeup_reason();

  // set keyblocks interrupt
  pinMode(KEYBLOCKS_INT_PIN, INPUT_PULLUP);
  attachInterrupt(KEYBLOCKS_INT_PIN, keyblock_interrupt, FALLING);

  // buzzer as output
  pinMode(BUZZER_PIN, OUTPUT);

  // Lock
  front_door_setup();

  // server
  server_setup();

  // NFC
  NFC_setup();

  // scanner_setup();
  // SDcard_test();
}

void loop()
{ // server
  server_loop();
  // front door
  front_door_loop();
  // keyblocks
  keyblock_loop();

  // test keyblock
  // KBlk.set(0b11111111);
  // Serial.println(KBlk.read(), BIN);
  // KBlk.test_keyblock();

  // scanner_loop();

  /*
  // buzzer test
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
  digitalWrite(BUZZER_PIN, LOW);
  delay(50);*/
}
