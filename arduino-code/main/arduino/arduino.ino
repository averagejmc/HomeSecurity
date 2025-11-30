#include <SPI.h>
#include <MFRC522.h>

#include "DoorSensor.h"
#include "MotionSensor.h"
#include "RFIDSensor.h"

const int RST_PIN = 22;   // Reset pin for RFID sensor
const int SS_PIN = 5;   // SDA/SS pin for RFID sensotr
const int motionPin = 14;  // PIR sensor
const int doorPin   = 12;  // Magnetic door sensor

DoorSensor doorSensor(doorPin);
MotionSensor motionSensor(motionPin);
RFIDSensor rfidSensor(SS_PIN, RST_PIN);

unsigned long lastLooptTime = 0;
const unsigned long loopInterval = 2000;
unsigned long lastBlink = 0;
const unsigned long blinkInterval = 500;
bool ledState = LOW;

void setup() {
  Serial.begin(115200);

  doorSensor.begin();
  motionSensor.begin();
  rfidSensor.begin();

  Serial.println(F("System Ready: RFID + Motion + Door"));
}

void loop() {
  if (!client.connected()) reconnectMQTT();
  client.loop();

  if (millis() - lastLooptTime => loopInterval) {
    lastLooptTime = millis();

    motionSensor.update();
    if (motionSensor.hasStateChanged()) {
      String motionMsg = motionSensor.getMessage();
      Serial.println(motionMsg);
      client.publish("home/motion", motionMsg.c_str());
    }

    doorSensor.update();
    if (doorSensor.hasStateChanged()) {
        String doorMsg = doorSensor.getMessage();
        Serial.println(doorMsg);
        client.publish("home/door", doorMsg.c_str());
    }

    String uid = rfidSensor.read();
    if (!uid.isEmpty()) {
        Serial.println("RFID UID: " + uid);
        client.publish("home/rfid", uid.c_str());
    }
  }
}
