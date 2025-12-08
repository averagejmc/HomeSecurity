#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <Servo.h>

#include "DoorSensor.h"
#include "MotionSensor.h"
#include "RFIDSensor.h"

const int ARDUINO_RX_PIN = 4;
const int ARDUINO_TX_PIN = 3;
const int RST_PIN = 9;   // Reset pin for RFID sensor
const int SS_PIN = 10;   // SDA/SS pin for RFID sensotr
const int motionPin = 8;  // PIR sensor
const int doorPin  = 5;  // Magnetic door sensor
const int servoPin = 6;
const int solenoidPin = 7;
const int switchPin = 2;
//const int ledPin = 5;

const int SERVO_LEFT = 70;
const int SERVO_STOP = 90;
const int SERVO_RIGHT = 110;
int currentServoSpeed = SERVO_STOP;

SoftwareSerial espSerial(ARDUINO_RX_PIN, ARDUINO_TX_PIN);
DoorSensor doorSensor(doorPin);
MotionSensor motionSensor(motionPin);
RFIDSensor rfidSensor(SS_PIN, RST_PIN);
Servo myServo;

unsigned long lastHeartbeat = 0;
const unsigned long heartbeatInterval = 30000;

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 5000;

const unsigned long SOLENOID_DURATION = 3000; 
const unsigned long SERVO_DURATION = 2500;
const unsigned long HOLD_DURATION = 15000;

unsigned long sequenceStartTime = 0;
bool isSequenceActive = false;

//unsigned long lastBlink = 0;
//const unsigned long blinkInterval = 1000;
//bool ledState = LOW;

String lastUid = "";

enum SystemMode { AUTO = 0, MAINTENANCE, SLEEP, MODE_COUNT };
volatile SystemMode currentMode = AUTO;

void switchMode() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  if (currentMode == SLEEP || interruptTime - lastInterruptTime > 200) {
    currentMode = static_cast<SystemMode>((currentMode + 1) % MODE_COUNT);
    lastInterruptTime = interruptTime;
  }
}

void sendDataToESP(String uid) {
  myServo.detach();
  
  espSerial.print(String(currentMode));
  espSerial.print(F(","));
  espSerial.print(motionSensor.getMessage());
  espSerial.print(F(","));
  espSerial.print(doorSensor.getMessage());
  espSerial.print(F(","));
  if (uid == "") {
    espSerial.println("No RFID"); 
  } else {
    espSerial.println(uid);
  }

  delay(10);

  myServo.attach(servoPin);
  myServo.write(currentServoSpeed);

  Serial.println(F("ARDUINO: Sent data to ESP"));
}

void setup() {
  Serial.begin(115200);
  espSerial.begin(9600);

  pinMode(switchPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(switchPin), switchMode, FALLING);

//  pinMode(ledPin, OUTPUT);
//  digitalWrite(ledPin, LOW);

  doorSensor.begin();
  motionSensor.begin();
  rfidSensor.begin();

  myServo.attach(servoPin);
  myServo.write(SERVO_STOP);

  digitalWrite(solenoidPin, HIGH);
  pinMode(solenoidPin, OUTPUT);

  Serial.println(F("ARDUINO: System Ready"));
}

void checkSequence() {
  if (!isSequenceActive) return; 

  unsigned long elapsed = millis() - sequenceStartTime;
  
  if (elapsed < SOLENOID_DURATION) {
    digitalWrite(solenoidPin, LOW); 
  } else {
    digitalWrite(solenoidPin, HIGH); 
  }
  
  if (elapsed < SERVO_DURATION) {
    currentServoSpeed = SERVO_RIGHT;
    myServo.write(SERVO_RIGHT);
  } 
  else if (elapsed < (SERVO_DURATION + HOLD_DURATION)) {
    currentServoSpeed = SERVO_STOP;
    myServo.write(SERVO_STOP); 
  } 
  else if (elapsed < (SERVO_DURATION + HOLD_DURATION + SERVO_DURATION)) {
    currentServoSpeed = SERVO_LEFT;
    myServo.write(SERVO_LEFT);  
  } 
  else {
    currentServoSpeed = SERVO_STOP;
    myServo.write(SERVO_STOP); 
    isSequenceActive = false; 
    Serial.println(F("Sequence Complete (Closed)."));
  }
}

void loop() {
  motionSensor.update();
  doorSensor.update();
  
  String rawUid = rfidSensor.read();
  String currentUid = "";

  if (rawUid != "") {
    currentUid = rawUid;
  }

  checkSequence();

  bool eventDetected = false;

  if (motionSensor.hasStateChanged()) {
    if (!isSequenceActive) { 
        eventDetected = true;
        Serial.println(F(">>> MOTION DETECTED! <<<")); 
    }
  }
  if (doorSensor.hasStateChanged()) eventDetected = true;
  if (currentUid != lastUid) {
    if (currentUid != "") {
      eventDetected = true;
      if (!isSequenceActive) {
        isSequenceActive = true;
        sequenceStartTime = millis();
      }
    }
    lastUid = currentUid;
  }
  
  switch (currentMode) {
    case AUTO:
//     digitalWrite(ledPin, LOW);
     if (eventDetected || (millis() - lastHeartbeat >= heartbeatInterval)) {
        lastHeartbeat = millis();
        if (currentUid != "") {
            sendDataToESP(currentUid);
        } else {
            sendDataToESP("");
        }
      }
      break;

    case MAINTENANCE:
//      if (millis() - lastBlink >= blinkInterval) {
//        lastBlink = millis();
//        ledState = !ledState;
//        digitalWrite(ledPin, ledState);
//      }

      if (millis() - lastSendTime >= sendInterval) {
        lastSendTime = millis();
        sendDataToESP(lastUid);
      }
      break;

    case SLEEP:
//      digitalWrite(ledPin, HIGH); 
      sendDataToESP(lastUid);
      delay(100);
      
      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
      sleep_enable();
      sleep_cpu();
      sleep_disable(); 
      break;

    default:
      break;
  }
}
