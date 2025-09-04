#pragma once

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>

class WiFiManager
{
public:
    WiFiManager(const char* ssid, const char* password, uint8_t hour, uint8_t minute, uint8_t duration);

    void begin();
    void update(time_t currentTime);

private:
    bool startAP();
    void stopAP();

    inline IPAddress getIP() const { return WiFi.softAPIP(); }   // Get current AP IP
    inline bool isRunning() const { return _apRunning; }          // Check if AP is active
    void serveLogFile(AsyncWebServerRequest *request, const char* path);

    inline bool isInWindow(uint16_t currentMinutes) const { return currentMinutes >= _startMinutes && currentMinutes < _endMinutes; }

private:
    const char* _ssid;
    const char* _password;
    bool _apRunning;

    uint16_t _startMinutes;
    uint16_t _endMinutes;

    AsyncWebServer _server {80};
};

