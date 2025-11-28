#ifndef SYSTEM_CONTROLLER_H
#define SYSTEM_CONTROLLER_H

#include <Arduino.h>
#include <WiFi.h>
#include "MqttHandler.h"
#include "DoorSensor.h"
#include "MotionSensor.h"
#include "RFIDSensor.h"

class SystemController {
public:
  SystemController(const char* ssid, const char* password, const char* mqttServer);
  void begin();
  void loop();

private:
  const char* _ssid;
  const char* _password;
  const char* _mqttServer;

  WiFiClient _espClient;
  MqttHandler _mqtt;

  // Sensor pins (kept as constants inside controller)
  static const int RST_PIN = 22;
  static const int SS_PIN = 5;
  static const int MOTION_PIN = 14;
  static const int DOOR_PIN = 12;

  DoorSensor _door;
  MotionSensor _motion;
  RFIDSensor _rfid;

  unsigned long _lastLoopTime;
  const unsigned long _loopInterval = 100;
};

#endif // SYSTEM_CONTROLLER_H
