#pragma once

#include <debouncer.h>
#include <FS.h>

class Sensor : Debouncer
{
private:
  uint16_t _intervalSec;
  uint16_t _lastOffset;
  uint32_t _startTime;
  volatile uint16_t _pulseCount;

  File _logFile;

public:
  Sensor(uint8_t pin, uint16_t millisInterval, uint16_t intervalSec);
  ~Sensor();

  void begin(int pinMode) override;
  void update(time_t currentTime);
  void update() override;
  
  void emptyLogFile();

private:
  void closeLogFile();
  void createLogFile();

  void resetTimestamp(time_t currentTime);
  void writeLogEntry(uint16_t offset);

  inline uint32_t calcOffset(time_t currentTime);
};

extern const char _sensorlog_name[];

