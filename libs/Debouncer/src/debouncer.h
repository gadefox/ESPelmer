/*
  The MIT License (MIT)

  Copyright (c) 2013 thomasfredericks

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <Arduino.h>

#define STATE_DEBOUNCED 0b00000001    // Final returned calculated debounced state
#define STATE_UNSTABLE  0b00000010    // Actual last state value behind the scene
#define STATE_CHANGED   0b00000100    // The DEBOUNCED_STATE has changed since last update()

/**
     @brief  The Debouce class. Just the deboucing code separated from all harware.
*/
class Debouncer
{
protected:
    inline void changeState();

public:
/*!
    @brief  Create an instance of the Debounce class.
    @endcode
*/
    Debouncer(uint8_t pin, uint16_t millisInterval);
  
    virtual void begin(int pinMode);

/*!
    @brief   Updates the pin's state. 
    Because Bounce does not use interrupts, you have to "update" the object before reading its value and it has to be done as often as possible (that means to include it in your loop()). Only call update() once per loop().
    @return True if the pin changed state.
*/
    virtual void update();

/**
     @brief Returns the pin's state (HIGH or LOW).
     @return HIGH or LOW.
*/
    inline bool read() const { return _state & STATE_DEBOUNCED; };

/**
    @brief Returns true if pin signal transitions from high to low.
*/
    inline bool fell() const { return (_state & STATE_DEBOUNCED) == 0 && _state & STATE_CHANGED; }

/**
    @brief Returns true if pin signal transitions from low to high.
*/
    inline bool rose() const { return _state & (STATE_DEBOUNCED | STATE_CHANGED); }

protected:
  uint8_t _pin;
  uint8_t _state;

  unsigned long _millisPrevious;
  uint16_t _millisInterval;

  unsigned long _stateChangeLastTime;
  unsigned long _durationOfPreviousState;
};

class PromptDebouncer : public Debouncer
{
public:
  void update() override;
};

class LockOutDebouncer : public Debouncer
{
public:
  void begin(int pinMode) override;
  void update() override;
};

