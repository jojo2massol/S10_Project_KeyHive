#pragma once

#include "server_mode.h"
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include "ArduinoJSON.h"
#include "arduino.h"
//#include <esp_task_wdt.h>

// 120 seconds WDT
// #define WDT_TIMEOUT 120


inline void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}
void server_mode();
