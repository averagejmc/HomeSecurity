#include "MqttHandler.h"
#include <WiFi.h>

MqttHandler::MqttHandler(Client &networkClient, const char *server, uint16_t port)
  : client(networkClient), server(server), port(port) {
}

void MqttHandler::begin() {
  client.setServer(server, port);
}

void MqttHandler::setClientId(const char *id) {
  clientId = id;
}

bool MqttHandler::ensureConnected() {
  if (client.connected()) return true;

  unsigned long now = millis();
  if (now - lastAttempt < retryInterval) return false;
  lastAttempt = now;

  // Try to connect with the configured client ID
  if (client.connect(clientId)) {
    return true;
  }

  return false;
}

bool MqttHandler::publish(const char *topic, const char *payload) {
  if (!ensureConnected()) return false;
  return client.publish(topic, payload);
}

bool MqttHandler::publish(const char *topic, const String &payload) {
  return publish(topic, payload.c_str());
}

void MqttHandler::loop() {
  client.loop();
}
