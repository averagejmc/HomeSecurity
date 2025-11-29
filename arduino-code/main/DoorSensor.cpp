<<<<<<< HEAD
#include "DoorSensor.h"

DoorSensor::DoorSensor(int pin)
  : _pin(pin),
    _lastStableState(HIGH),
    _lastReading(HIGH),
    _previousState(HIGH),
    _lastDebounceTime(0) {}

void DoorSensor::begin() {
  pinMode(_pin, INPUT_PULLUP); 
}

void DoorSensor::update() {
  int reading = digitalRead(_pin);

  if (reading != _lastReading) {
    _lastDebounceTime = millis(); 
  }

  if ((millis() - _lastDebounceTime) > _debounceDelay) {
    _lastStableState = reading;
  }

  _lastReading = reading;
}

bool DoorSensor::hasStateChanged() {
  if (_lastStableState != _previousState) {
    _previousState = _lastStableState;
    return true;
  }
  return false;
}

int DoorSensor::getState() const {
  return _lastStableState;
}

String DoorSensor::getMessage() const {
  if (_lastStableState == LOW) {
    return "Door open";
  } else {
    return "Door closed!";
  }
}
=======
#include "DoorSensor.h"

DoorSensor::DoorSensor(int pin)
  : _pin(pin),
    _lastStableState(HIGH),
    _lastReading(HIGH),
    _previousState(HIGH),
    _lastDebounceTime(0) {}

void DoorSensor::begin() {
  pinMode(_pin, INPUT_PULLUP); 
}

void DoorSensor::update() {
  int reading = digitalRead(_pin);

  if (reading != _lastReading) {
    _lastDebounceTime = millis(); 
  }

  if ((millis() - _lastDebounceTime) > _debounceDelay) {
    _lastStableState = reading;
  }

  _lastReading = reading;
}

bool DoorSensor::hasStateChanged() {
  if (_lastStableState != _previousState) {
    _previousState = _lastStableState;
    return true;
  }
  return false;
}

int DoorSensor::getState() const {
  return _lastStableState;
}

String DoorSensor::getMessage() const {
  if (_lastStableState == LOW) {
    return "Door open";
  } else {
    return "Door closed!";
  }
}
>>>>>>> 27dc04e0428cb78814e1be60a14c747cebfc24e0
