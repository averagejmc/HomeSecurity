#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include "DoorSensor.h"
#include "MotionSensor.h"
#include "RFIDSensor.h"

const int ESP_RX_PIN = 3;
const int ESP_TX_PIN = 4;
const int RST_PIN = 22;   // Reset pin for RFID sensor
const int SS_PIN = 5;   // SDA/SS pin for RFID sensotr
const int motionPin = 14;  // PIR sensor
const int doorPin   = 12;  // Magnetic door sensor
const int switchPin = 2;
const int ledPin = 9;

SoftwareSerial espSerial(ESP_RX_PIN, ESP_TX_PIN);

DoorSensor doorSensor(doorPin);
MotionSensor motionSensor(motionPin);
RFIDSensor rfidSensor(SS_PIN, RST_PIN);

unsigned long lastLooptTime = 0;
const unsigned long loopInterval = 2000;
unsigned long lastBlink = 0;
const unsigned long blinkInterval = 500;
bool ledState = LOW;

enum SystemMode { AUTO = 0, MAINTENANCE, SLEEP, MODE_COUNT };
volatile SystemMode currentMode = AUTO;

void switchMode() {
  currentMode = static_cast<SystemMode>((currentMode + 1) % MODE_COUNT);
}

void setup() {
  Serial.begin(115200);
  espSerial.begin(9600);

  pinMode(switchPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(switchPin), switchMode, FALLING);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  doorSensor.begin();
  motionSensor.begin();
  rfidSensor.begin();

  Serial.println(F("System Ready: RFID + Motion + Door"));
}

void loop() {

  switch (currentMode) {
    case AUTO:
      motionSensor.update();
      doorSensor.update();
  
      String uid = rfidSensor.read();
  
      if (millis() - lastLooptTime >= loopInterval) {
        lastLooptTime = millis();
  
        String motionState = motionSensor.getMessage();
        String doorState = doorSensor.getMessage();

        // print data in serial in format 'mode,motion,door,rfid'
        String dataLine = String(currentMode) + "," +
                                      motionState + "," +
                                      doorState + "," +
                                      uid;
  
        espSerial.println(dataLine);
      }
      break;

    case MAINTENANCE:
      if (millis() - lastBlink >= blinkInterval) {
        lastBlink = millis();
        ledState = !ledState;
        digitalWrite(ledPin, ledState);
      }

      motionSensor.update();
      doorSensor.update();
      rfidSensor.read();
      break;

    case SLEEP:
      digitalWrite(ledPin, HIGH); 

      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      sleep_enable();
    
      sleep_cpu();

      sleep_disable(); 
    
      break;

    default:
      break;
  }
}
