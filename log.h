#pragma once

struct LogEntry {
  uint16_t offset;
  uint16_t pulses;
};

// time_t realTime = startTime + (offset30s * 30);
