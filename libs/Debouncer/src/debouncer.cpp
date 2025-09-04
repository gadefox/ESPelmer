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

#include "debouncer.h"

// DEBOUNCER
Debouncer::Debouncer(uint8_t pin, uint16_t millisInterval)
{
    _pin = pin;
    _millisInterval = millisInterval;
}

void Debouncer::begin(int mode)
{
    pinMode(_pin, mode);

    _state = 0;
    if (digitalRead(_pin))
        _state |= STATE_DEBOUNCED | STATE_UNSTABLE;

    _millisPrevious = millis();
}

void Debouncer::update()
{
    _state &= ~STATE_CHANGED;

    // Read the state of the switch in a temporary variable.
    bool currentState = digitalRead(_pin);

    // If the reading is different from last reading, reset the debounce counter
    if (currentState != (_state & STATE_UNSTABLE)) {
        _millisPrevious = millis();
        _state ^= STATE_UNSTABLE;
    } else if (millis() - _millisPrevious >= _millisInterval) {
        // We have passed the threshold time, so the input is now stable
        // If it is different from last state, set the STATE_CHANGED flag
        if (currentState != _state & STATE_DEBOUNCED) {
            _millisPrevious = millis();
            changeState();
        }
    }
}

void Debouncer::changeState()
{
	_state ^= STATE_DEBOUNCED;
	_state |= STATE_CHANGED;

    _durationOfPreviousState = millis() - _stateChangeLastTime;
	_stateChangeLastTime = millis();
}

// Prompt detection debouncer
void PromptDebouncer::update()
{
    _state &= ~STATE_CHANGED;

    // Read the state of the switch port into a temporary variable.
    bool readState = digitalRead(_pin);
    if (readState != (_state & STATE_DEBOUNCED)) {
      // We have seen a change from the current button state.
      if (millis() - _millisPrevious >= _millisInterval) {
        // We have passed the time threshold, so a new change of state is allowed.
        // set the STATE_CHANGED flag and the new DEBOUNCED_STATE.
        // This will be prompt as long as there has been greater than interval_misllis ms since last change of input.
        // Otherwise debounced state will not change again until bouncing is stable for the timeout period.
        changeState();
      }
    }

    // If the readState is different from previous readState, reset the debounce timer - as input is still unstable
    // and we want to prevent new button state changes until the previous one has remained stable for the timeout.
    if (readState != (_state & STATE_UNSTABLE)) {
        // Update Unstable Bit to macth readState
        _state ^= STATE_UNSTABLE;
        _millisPrevious = millis();
    }
}

// Lock out debouncer
void LockOutDebouncer::begin(int pinMode)
{
  Debouncer::begin(pinMode);

  _millisPrevious = 0;
}

void LockOutDebouncer::update()
{
    _state &= ~STATE_CHANGED;

    // Ignore everything if we are locked out
    if (millis() - _millisPrevious >= _millisInterval) {
        bool currentState = digitalRead(_pin);
        if (currentState != (_state & STATE_DEBOUNCED)) {
            _millisPrevious = millis();
            changeState();
        }
    }
}

