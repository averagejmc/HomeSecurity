#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>

#include "DoorSensor.h"
#include "MotionSensor.h"
#include "RFIDSensor.h"

const char* ssid = "Forger Hideout";
const char* password = "AnyaLoidYor";

const char* mqtt_server = "192.168.68.60";

WiFiClient espClient;
PubSubClient client(espClient);

const int RST_PIN = 22;   // Reset pin for RFID sensor
const int SS_PIN = 5;   // SDA/SS pin for RFID sensotr
const int motionPin = 14;  // PIR sensor
const int doorPin   = 12;  // Magnetic door sensor

DoorSensor doorSensor(doorPin);
MotionSensor motionSensor(motionPin);
RFIDSensor rfidSensor(SS_PIN, RST_PIN);

unsigned long lastLooptTime = 0;
const unsigned long loopInterval = 100;

unsigned long lastMQTT = 0;
const unsigned long mqttInterval = 1000;

void setupWiFi() {
  Serial.println("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();
  const unsigned long timeout = 10000; 
  unsigned long lastPrint = 0;  

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
    if (millis() - lastPrint >= 500) {
      Serial.print(".");
      lastPrint = millis();
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected! IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nWi-Fi connection failed! Timeout reached.");
  }
}

void reconnectMQTT() {
  if (client.connected()) return;
  
  if (millis() - lastMQTT => mqttInterval){
    lastMQTT = millis();

    if(client.connect("ESP32_RFID_CLIENT")) {
      Serial.println("Connected to broker!");
    } else {
      Serial.print("Failed MQTT connection, rc=");
      Serial.print(client.state());
    }

  }
}

void setup() {
  Serial.begin(115200);

  setupWiFi();
  client.setServer(mqtt_server, 1883);

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

    // ---- Motion Sensor (executes only on state change) ----
    motionSensor.update();
    if (motionSensor.hasStateChanged()) {
      String motionMsg = motionSensor.getMessage();
      Serial.println(motionMsg);
      client.publish("home/motion", motionMsg.c_str());
    }

    // ---- Door Sensor (same here) ----
    doorSensor.update();
    if (doorSensor.hasStateChanged()) {
        String doorMsg = doorSensor.getMessage();
        Serial.println(doorMsg);
        client.publish("home/door", doorMsg.c_str());
    }

    // ---- RFID (executes only when a UID is read) ----
    String uid = rfidSensor.read();
    if (!uid.isEmpty()) {
        Serial.println("RFID UID: " + uid);
        client.publish("home/rfid", uid.c_str());
    }
  }
}