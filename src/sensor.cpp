#include <eventlog.h>
#include <LittleFS.h>

#include "sensor.h"

// Global implementation
const char _sensorlog_path[] = "/sensor.bin";

// Constructor takes sensor pin and pointer to Event
Sensor::Sensor(uint8_t pin, uint16_t millisInterval, uint16_t intervalSec)
    : Debouncer(pin, millisInterval)
{
  _intervalSec = intervalSec;
  _pulseCount = 0;
  _lastOffset = 0;
}

Sensor::~Sensor()
{
  if (_maxEntries > 0)
      writeLogEntryBuffer(_maxEntries);

  if (_logFile)
    _logFile.close();
}

// Open log file in append mode
void Sensor::begin(int pinMode)
{
  Debouncer::begin(pinMode);

  createLogFile();
}

void Sensor::createLogFile()
{
  _logFile = LittleFS.open(_sensorlog_path, "a");
  if (!_logFile)
    _events.log(EventLog::ERROR, "Failed to open log file");
}

void Sensor::emptyLogFile()
{
  LittleFS.remove(_sensorlog_path);
  createLogFile();
}

void Sensor::writeLogEntryBuffer(size_t count)
{
    size_t written = _logFile.write((const uint8_t*)_entries, sizeof(LogEntry) * count);
    if (written != sizeof(LogEntry) * count)
      _events.log(EventLog::ERROR, "Failed to write log buffer");

    _logFile.flush();
    _maxEntries = 0;
}

void Sensor::resetTimestamp(time_t currentTime)
{
    SegmentMarker marker { UINT16_MAX, currentTime };

    _startTime = currentTime;
    _lastOffset = 0;

    if (_maxEntries > 0) {
        writeLogEntryBuffer(_maxEntries);
        _maxEntries = 0;
    }

    // Write delimiter and startTime as binary at start of file (4 or 8 bytes)
    _logFile.write((const uint8_t*)&marker, sizeof(SegmentMarker));
    _logFile.flush();

    _events.log(EventLog::INFO, "Sensor: reset timestamp at %lu", currentTime);
}

void Sensor::saveLogEntry(uint16_t offset)
{
    uint16_t count;

    _entries[_maxEntries].offset = offset;
    _lastOffset = offset;

    noInterrupts();
    _entries[_maxEntries].pulses = _pulseCount;
    _pulseCount = 0;
    interrupts();

    // Log info
    if (++_maxEntries == BUFFER_SIZE)
        writeLogEntryBuffer(BUFFER_SIZE);
}

void Sensor::update()
{
  // Detect rising edge (or falling edge depending on sensor)
  Debouncer::update();
  if (fell())
    _pulseCount++;
}

uint32_t Sensor::calcOffset(time_t currentTime)
{
  uint32_t elapsed = currentTime - _startTime;  // seconds
  return elapsed / _intervalSec;
}

void Sensor::update(time_t currentTime)
{
    uint32_t offset = calcOffset(currentTime);

    // Update debouncer
    update();

    if (offset == _lastOffset)
      return;

    if (offset >= UINT16_MAX) {
      resetTimestamp(currentTime);
      // offset is now 0
    }

    saveLogEntry(offset);
}
