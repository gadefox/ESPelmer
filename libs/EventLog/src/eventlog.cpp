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

#include <LittleFS.h>
#include <stdarg.h>

#include "eventlog.h"

// Global implementation
EventLog _events;
const char _eventlog_path[] = "/events.log";
const char _monthAbbreviations[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

// Class implementation
EventLog::EventLog()
{
  _day = 0;
  _month = UINT8_MAX;
  _year = 0;
}

EventLog::~EventLog()
{
    if (_logFile)
      _logFile.close();
}

// Open log file in append mode
bool EventLog::begin()
{
    time_t now = time(nullptr);
    struct tm *t = localtime(&now);

    _logFile = LittleFS.open(_eventlog_path, "a");
    if (!_logFile)
      return false;

    writeHeader(t->tm_mday, t->tm_mon, t->tm_year);
    _logFile.flush();
    return true;
}

// Convert enum Level to string without default case
const char EventLog::levelToChar(Level level)
{
    if (level == ERROR)
      return 'E';
    if (level == WARN)
      return 'W';
    return 'I';
}

void EventLog::writeHeader(uint8_t day, uint8_t month, uint8_t year)
{
    if (_year != year) {
      _year = year;
      _logFile.printf("%d\n", year + 1900);
    }

    if (_month != month) {
      _month = month;
      _logFile.printf("%s\n", _monthAbbreviations[month]);
    }

    if (_day != day) {
      _day = day;
      _logFile.printf("%02d\n", day);
    }
}

// Log a message with level and timestamp
void EventLog::writeMessage(Level level, const char* message)
{
    time_t now = time(nullptr);
    struct tm *t = localtime(&now);

    writeHeader(t->tm_mday, t->tm_mon, t->tm_year);
    _logFile.printf("%c %02d%02d%02d %s\n", levelToChar(level), t->tm_hour, t->tm_min, t->tm_sec, message);
    _logFile.flush();
}

void EventLog::log(Level level, const char* format, ...)
{
  char buffer[256];

  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  // Call original log method with formatted string
  writeMessage(level, buffer);
}

bool EventLog::emptyLogFile()
{
  _day = 0;
  _month = UINT8_MAX;
  _year = 0;

  LittleFS.remove(_eventlog_path);
  return begin();
}

