#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <Arduino.h>
#include <PubSubClient.h>

class MqttHandler {
public:
  MqttHandler(Client &networkClient, const char *server, uint16_t port = 1883);
  void begin();
  void setClientId(const char *id);
  bool ensureConnected();
  bool publish(const char *topic, const char *payload);
  bool publish(const char *topic, const String &payload);
  void loop();

private:
  PubSubClient client;
  const char *server;
  uint16_t port;
  const char *clientId = "ESP32_CLIENT";
  unsigned long lastAttempt = 0;
  unsigned long retryInterval = 1000; // ms
};

#endif // MQTT_HANDLER_H
