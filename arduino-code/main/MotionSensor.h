<<<<<<< HEAD
#ifndef MOTIONSENSOR_H
#define MOTIONSENSOR_H

#include <Arduino.h>

class MotionSensor {
public:
  MotionSensor(int pin);
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
  const unsigned long _debounceDelay = 200;
};

#endif
=======
#ifndef MOTIONSENSOR_H
#define MOTIONSENSOR_H

#include <Arduino.h>

class MotionSensor {
public:
  MotionSensor(int pin);
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
  const unsigned long _debounceDelay = 200;
};

#endif
>>>>>>> 27dc04e0428cb78814e1be60a14c747cebfc24e0
