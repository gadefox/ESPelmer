/*
  EventLog - imple and flexible event logging utility for Arduino projects.
  Copyright (c) 2025 gadefoxren@gmail.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <Arduino.h>
#include <FS.h>

class EventLog
{
public:
  enum Level { INFO, WARN, ERROR };

  EventLog();
  ~EventLog();

  bool begin();
  bool emptyLogFile();

  void log(Level level, const char* format, ...);

private:
  const char levelToChar(Level level);
  void writeMessage(Level level, const char* message);
  void writeHeader(uint8_t day, uint8_t month, uint8_t year);

  File _logFile;
  uint8_t _day;
  uint8_t _month;
  uint8_t _year;
};

extern EventLog _events;
extern const char _eventlog_path[];

