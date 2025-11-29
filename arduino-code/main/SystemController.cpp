#include "SystemController.h"
#include <SPI.h>

SystemController::SystemController(const char* ssid, const char* password, const char* mqttServer)
  : _ssid(ssid), _password(password), _mqttServer(mqttServer), _espClient(), _mqtt(_espClient, mqttServer),
    _door(DOOR_PIN), _motion(MOTION_PIN), _rfid(SS_PIN, RST_PIN), _lastLoopTime(0) {
}

void SystemController::begin() {
  Serial.begin(115200);

  // Wi-Fi
  Serial.println("Connecting to Wi-Fi");
  WiFi.begin(_ssid, _password);

  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 10000;
  unsigned long lastPrint = 0;

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    if (millis() - lastPrint >= 500) {
      Serial.print('.');
      lastPrint = millis();
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected! IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nWi-Fi connection failed! Timeout reached.");
  }

  // MQTT
  _mqtt.begin();
  _mqtt.setClientId("ESP32_RFID_CLIENT");

  // Sensors
  _door.begin();
  _motion.begin();
  _rfid.begin();

  Serial.println(F("System Ready: RFID + Motion + Door"));
}

void SystemController::loop() {
  _mqtt.ensureConnected();
  _mqtt.loop();

  if (millis() - _lastLoopTime >= _loopInterval) {
    _lastLoopTime = millis();

    // Motion
    _motion.update();
    if (_motion.hasStateChanged()) {
      String msg = _motion.getMessage();
      Serial.println(msg);
      _mqtt.publish("home/motion", msg);
    }

    // Door
    _door.update();
    if (_door.hasStateChanged()) {
      String msg = _door.getMessage();
      Serial.println(msg);
      _mqtt.publish("home/door", msg);
    }

    // RFID
    String uid = _rfid.read();
    if (uid.length() > 0) {
      Serial.println("RFID UID: " + uid);
      _mqtt.publish("home/rfid", uid);
    }
  }
}
