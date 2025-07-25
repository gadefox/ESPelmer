#include <debouncer.h>
#include <eventlog.h>
#include <LittleFS.h>

#include "global.h"
#include "wifi.h"

ColorLED _led(D1, D2, D5);
Sensor _sensor(D6, 15, 30);
WiFiManager _wifi("elmer", "1", 12, 00, 20);  // 12:00-12:20

void setup()
{
  Serial.begin(115200);

  // Initialize LED first due to error blinking
  _led.begin();

  if (!LittleFS.begin()) {
    Serial.println(F("Failed to initialize file system"));
    _led.error();
  }

  if (!_events.begin())
    _led.error();

  _sensor.begin(INPUT_PULLUP);
  _wifi.begin();
  _events.log(EventLog::INFO, "System started");
}

void loop()
{
  time_t now = time(nullptr);

  _led.update();
  _sensor.update(now);
  _wifi.update(now);
}
