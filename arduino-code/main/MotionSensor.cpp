#include "MotionSensor.h"

MotionSensor::MotionSensor(int pin)
  : _pin(pin),
    _lastStableState(LOW),
    _lastReading(LOW),
    _previousState(LOW),
    _lastDebounceTime(0) {}

void MotionSensor::begin() {
  pinMode(_pin, INPUT);
}

void MotionSensor::update() {
  int reading = digitalRead(_pin);

  if (reading != _lastReading) {
    _lastDebounceTime = millis();
  }

  if ((millis() - _lastDebounceTime) > _debounceDelay) {
    _lastStableState = reading;
  }

  _lastReading = reading;
}

bool MotionSensor::hasStateChanged() {
  if (_lastStableState != _previousState) {
    _previousState = _lastStableState;
    return true;
  }
  return false;
}

int MotionSensor::getState() const {
  return _lastStableState;
}

String MotionSensor::getMessage() const {
  if (_lastStableState == HIGH) {
    return "Motion detected!";
  } else {
    return "No motion";
  }
}
