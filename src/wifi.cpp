#include <ArduinoOTA.h>
#include <LittleFS.h>
#include <eventlog.h>

#include "wifi.h"
#include "global.h"
#include "sensor.h"

WiFiManager::WiFiManager(const char* ssid, const char* password, uint8_t hour, uint8_t minute, uint8_t duration)
{
    _ssid = ssid;
    _password = password;

    _startMinutes = hour * 60 + minute;
    _endMinutes = _startMinutes + duration;

    _apRunning = false;
}

void WiFiManager::begin()
{
  // Initialize OTA
  ArduinoOTA.onStart([]() {
    _events.log(EventLog::ERROR, "OTA Start");
  });
  
  ArduinoOTA.onEnd([]() {
    _events.log(EventLog::ERROR, "OTA End");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    _events.log(EventLog::ERROR, "OTA Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    _events.log(EventLog::ERROR, "OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      _events.log(EventLog::ERROR, "Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      _events.log(EventLog::ERROR, "Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      _events.log(EventLog::ERROR, "Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      _events.log(EventLog::ERROR, "Receive Failed");
    else if (error == OTA_END_ERROR)
      _events.log(EventLog::ERROR, "End Failed");
  });

  // Initialize webserver
  _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head><title>Logs</title></head>
      <body>
        <h1>Download</h1>
        <a href="event-log"><button>Download Event Logs</button></a><br><br>
        <a href="sensor-log"><button>Download Sensor Logs</button></a>
        <a href="delete-logs"><button>Delete Logs</button></a>
      </body>
      </html>
    )rawliteral");
  });

  // Event log
  _server.on("/event-log", HTTP_GET, [this](AsyncWebServerRequest *request) {
      serveLogFile(request, _eventlog_name);
  });

  // Sensor log
  _server.on("sensor-log", HTTP_GET, [this](AsyncWebServerRequest *request) {
      serveLogFile(request, _sensorlog_name);
  });

  // Delete log
  _server.on("delete-logs", HTTP_GET, [this](AsyncWebServerRequest *request) {
      _events.emptyLogFile();
      _sensor.emptyLogFile();
  });

  _server.begin();
}

bool WiFiManager::startAP()
{
    if (isRunning())
      return true;
    
    WiFi.mode(WIFI_AP);

    bool success = WiFi.softAP(_ssid, _password);
    if (success) {
        _events.log(EventLog::ERROR, "Failed to start Access Point");
        return false;
    }

    _apRunning = true;
    _server.begin();
    
    ArduinoOTA.begin();

    _events.log(EventLog::INFO, "Access Point started. IP: %s", WiFi.softAPIP().toString().c_str());
    return true;
}

void WiFiManager::stopAP()
{
    if (!isRunning())
      return;
    
    _apRunning = false;

    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);

    _events.log(EventLog::INFO, "Access Point stopped");
}

void WiFiManager::update(time_t currentTime)
{
    struct tm* now = localtime(&currentTime);
    int currentMinutes = now->tm_hour * 60 + now->tm_min;

    if (!isInWindow(currentMinutes)) {
      stopAP();
      return;
    }

    if (startAP())
      ArduinoOTA.handle();
}

void WiFiManager::serveLogFile(AsyncWebServerRequest *request, const char* path)
{
  File file = LittleFS.open(path, "r");
  if (file)
    request->send(file, String(path), "application/octet-stream");
  else
    request->send(404, "text/plain", "Log file not found");
}
