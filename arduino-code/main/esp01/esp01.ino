#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "MqttHandler.h"  

MQTTClientHandler mqtt("192.168.69.60");

String incoming = "";

void setup() {
  Serial.begin(9600);
  mqtt.begin();

  Serial.println("ESP01 MQTT BRIDGE READY");
}

void loop() {
  mqtt.loop();

  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      processLine(incoming);
      incoming = "";
    } else {
      incoming += c;
    }
  }
}

void processLine(String line) {
  line.trim();
  if (line.length() == 0) return;

  // Split CSV
  int idx1 = line.indexOf(',');
  int idx2 = line.indexOf(',', idx1 + 1);
  int idx3 = line.indexOf(',', idx2 + 1);

  if (idx1 < 0 || idx2 < 0 || idx3 < 0) return;

  String currentMode  = line.substring(0, idx1);
  String motion = line.substring(idx1 + 1, idx2);
  String door   = line.substring(idx2 + 1, idx3);
  String rfid   = line.substring(idx3 + 1);

  // Publish
  mqtt.publish("home/mode", currentMode.c_str());
  mqtt.publish("home/motion", motion.c_str());
  mqtt.publish("home/door", door.c_str());

  if (rfid.length() > 0)
    mqtt.publish("home/rfid", rfid.c_str());

  Serial.println("Published: " + line);
}
