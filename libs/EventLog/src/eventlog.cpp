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
const char _eventlog_name[] = "/events.log";

EventLog::EventLog()
{
}

EventLog::~EventLog()
{
    if (_logFile)
      _logFile.close();
}

// Open log file in append mode
bool EventLog::begin()
{
    _logFile = LittleFS.open(_eventlog_name, "a");
    return _logFile;
}

// Convert enum Level to string without default case
const char* EventLog::levelToString(Level level)
{
    if (level == INFO)
      return "INFO";
    if (level == WARN)
      return "WARN";
    return "ERROR";
}

// Log a message with level and timestamp
void EventLog::logInternal(Level level, const char* message)
{
    time_t now = time(nullptr);
    struct tm *t = localtime(&now);

    _logFile.printf("[%04d-%02d-%02d %02d:%02d:%02d] %s: %s\n", 
       t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, levelToString(level), message);
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
  logInternal(level, buffer);
}

bool EventLog::emptyLogFile()
{
  LittleFS.remove(_eventlog_name);
  return begin();
}

