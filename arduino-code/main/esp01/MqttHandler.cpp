#include <Arduino.h>

#include "MqttHandler.h"

MQTTClientHandler::MQTTClientHandler(const char* server, uint16_t port)
  : server(server), port(port), client(espClient) {}

void MQTTClientHandler::begin() {
    client.setServer(server, port);
}

void MQTTClientHandler::loop() {
    if (!client.connected()) reconnect();
    client.loop();
}

void MQTTClientHandler::publish(const char* topic, const char* message) {
    if (client.connected()) {
        client.publish(topic, message);
    }
}

void MQTTClientHandler::reconnect() {
    if (client.connected()) return;

    unsigned long now = millis();
    if (now - lastReconnectAttempt >= reconnectInterval) {
        lastReconnectAttempt = now;

        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32_RFID_CLIENT")) {
            Serial.println("connected!");
        } else {
            Serial.print("failed, rc=");
            Serial.println(client.state());
        }
    }
}
