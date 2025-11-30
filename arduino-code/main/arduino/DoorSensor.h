#ifndef DOORSENSOR_H
#define DOORSENSOR_H

#include <Arduino.h>

class DoorSensor {
public:
  DoorSensor(int pin);
  void begin();
  void update();
  bool hasStateChanged();
  String getMessage() const;
  int getState() const;

private:
  int _pin;
  int _lastStableState;
  int _lastReading;
  int _previousState;
  unsigned long _lastDebounceTime;
  const unsigned long _debounceDelay = 50;
};

#endif
