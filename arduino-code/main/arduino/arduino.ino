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

void setup() {
  Serial.begin(115200);

  doorSensor.begin();
  motionSensor.begin();
  rfidSensor.begin();

  Serial.println(F("System Ready: RFID + Motion + Door"));
}

void loop() {
  motionSensor.update();
  doorSensor.update();

  String uid = rfidSensor.read();

  if (millis() - lastLooptTime >= loopInterval) {
    lastLooptTime = millis();

    String motionState = motionSensor.getMessage();
    String doorState = doorSensor.getMessage();

    // print data in serial in format 'mode,motion,door,rfid'
    String dataLine = motionState + "," +
                                  doorState + "," +
                                  uid;

    Serial.println(dataLine);
  }
}
