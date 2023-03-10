#include "server_mode.h"

#include "credentials.h"
bool busy = false;
AsyncWebServer server(80);

void server_setup()
{
  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    log_e("An Error has occurred while mounting SPIFFS");
    return;
  }
  // set watchdog
  esp_task_wdt_init(WDT_TIMEOUT, true); // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);               // add current thread to WDT watch

  // print ssid and password
  Serial.println("SSID:     " + String(ssid));
#if !HIDE_AP_PASSWORD
  Serial.println("password: " + String(password));
  Serial.println("QR: https://api.qrserver.com/v1/create-qr-code/?data=WIFI%3AT%3AWPA%3BS%3A" + String(ssid) + "%3BP%3A" + String(password) + "%3B%3B&size=512x512&margin=16");
#endif

  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP               ... ");
  Serial.println(WiFi.softAP(ssid, password) ? "Ready" : "Failed!");
  // WiFi.softAP(ssid);
  // WiFi.softAP(ssid, password, channel, ssdi_hidden, max_connection)

  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              // reset watchdog
              esp_task_wdt_reset();
              request->send(SPIFFS, "/index.html", String(), false);
              Serial.println("/ HTTP_GET /index.html"); });
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              // reset watchdog
              esp_task_wdt_reset();
              request->send(SPIFFS, "/index.html", String(), false);
              Serial.println("/ HTTP_GET /index.html"); });

  server.on("/pic", HTTP_GET, [](AsyncWebServerRequest *request)
            {
            // reset watchdog
            esp_task_wdt_reset();
            if(busy)
            {
              request->send(500, "text/plain", "Busy");
              Serial.println("/ HTTP_GET /pic Busy");
              return;
            }
            busy = true;
            Serial.println("/ HTTP_GET /pic");
            
            busy = false; });

  server.on("/list", HTTP_GET, [](AsyncWebServerRequest *request)
            {
      // reset watchdog
      esp_task_wdt_reset();
      Serial.println("/ HTTP_GET /list");

      WiFi.mode(WIFI_AP_STA);
      String text = "";
      text.reserve(1024); // Reserve some space to prevent ram fragmentation
      int n = WiFi.scanNetworks();
      if (n == 0)
      {
        text += "no networks found";
      }
      else
      {
        text += String(n);
        text += " networks found\n";
        // list the networks found
        for (int i = 0; i < n; ++i)
        {
          // Print SSID and RSSI for each network found
          text += "\n";
          text += String(i + 1);
          text += ": ";
          text += WiFi.SSID(i);
          text += " (";
          text += WiFi.RSSI(i);
          text += ")";
          text += (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*";
        }
      }
      Serial.println(text);
      request->send(200, "text/plain", text); });

  server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request)
            {
      // reset watchdog
      esp_task_wdt_reset();

      int paramsNr = request->params(); // number of params (e.g., 1)
      Serial.println(paramsNr);
      Serial.println();

      String ssid = request->getParam(0)->value();
      String password = request->getParam(1)->value();
      String ip = request->getParam(2)->value();
      Serial.println("ssid:     " + ssid);
      Serial.println("password: " + password);
      Serial.println("IP:       " + ip);

      // check if ssid and password are valid
      if (ssid.length() < 1 || password.length() < 8)
      {
        request->send(400, "text/plain", "Invalid ssid or password");
        return;
      }
      // check if ip is valid
      // parse ip with "." as delimiter and check if values are in range 0-255
      String ip_parts[4];
      int ip_parts_int[4];
      int i = 0;
      String iptemp = ip;
      for (int i = 0; i < 4; i++)
      {
        ip_parts[i] = iptemp.substring(0, iptemp.indexOf("."));
        ip_parts_int[i] = ip_parts[i].toInt();
        iptemp = iptemp.substring(iptemp.indexOf(".") + 1);
        if (ip_parts_int[i] < 0 || ip_parts_int[i] > 255)
        {
          request->send(400, "text/plain", "Invalid IP : out of range 0-255");
          return;
        }
      }

      request->send(200, "text/plain", "Saved, restarting...");

      delay(1000);
      // reset ESP32
      ESP.restart(); });

  server.on("/reboot", HTTP_ANY, [](AsyncWebServerRequest *request)
            {
    // reset watchdog
    esp_task_wdt_reset();
    request->send(200, "text/plain", "Restarting ...");

            
    delay(1000);
    // reset ESP32
    ESP.restart(); });
  server.onNotFound(notFound);
  server.begin();

  esp_task_wdt_reset();
}
void server_loop()
{
  uint clients = 0;
  // if clients are connected, reset watchdog
  if (WiFi.softAPgetStationNum())
    esp_task_wdt_reset();
  if (clients != WiFi.softAPgetStationNum())
  {
    clients = WiFi.softAPgetStationNum();
    Serial.println("Clients: " + String(clients));
  }
  delay(200);
}