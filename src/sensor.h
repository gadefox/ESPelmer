#pragma once

#include <debouncer.h>
#include <FS.h>

#define BUFFER_SIZE  1024

struct SegmentMarker {
  uint16_t delimiter;
  time_t timestamp;
};

struct LogEntry {
  uint16_t offset;
  uint16_t pulses;
};

class Sensor : Debouncer
{
private:
  uint16_t _intervalSec;
  uint16_t _lastOffset;
  uint32_t _startTime;
  volatile uint16_t _pulseCount;

  File _logFile;

  LogEntry _entries[BUFFER_SIZE];
  size_t _maxEntries;

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
  void saveLogEntry(uint16_t offset);
  void writeLogEntryBuffer(size_t count);

  inline uint32_t calcOffset(time_t currentTime);
};

extern const char _sensorlog_path[];

