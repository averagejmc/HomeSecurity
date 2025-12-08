#include <Arduino.h>

#include "MqttHandler.h"

MQTTClientHandler::MQTTClientHandler(const char* ssid, const char* password, const char* server, uint16_t port)
  : ssid(ssid), password(password), server(server), port(port), client(espClient) {}

void MQTTClientHandler::begin() {
    connectWifi();
    client.setServer(server, port);
}

void MQTTClientHandler::connectWifi() {
    delay(10);
    Serial.println();
    Serial.print(F("ESP01: Connecting to "));
    Serial.println(ssid);
  
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(F("."));
    }
  
    Serial.println(F("ESP01: "));
    Serial.println(F("ESP01: WiFi connected"));
}

void MQTTClientHandler::loop() {
    if (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }
  
    if (!client.connected()) reconnect();
    client.loop();
}

void MQTTClientHandler::publish(const char* topic, const char* message) {
    if (client.connected()) {
        client.publish(topic, message);
    }
}

void MQTTClientHandler::reconnect() {
    if (WiFi.status() != WL_CONNECTED) return;

    if (client.connected()) return;

    unsigned long now = millis();
    if (now - lastReconnectAttempt >= reconnectInterval) {
        lastReconnectAttempt = now;

        Serial.println(F("ESP01: Attempting MQTT connection..."));
        String clientId = "ESP01-";
        clientId += String(ESP.getChipId(), HEX);
        if (client.connect(clientId.c_str())) {
            Serial.println(F("ESP01: connected!"));
        } else {
            Serial.print(F("ESP01: failed, rc="));
            Serial.println(client.state());
        }
    }
}
