#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

class MQTTClientHandler {
public:
    MQTTClientHandler(const char* server, uint16_t port = 1883);
    
    void begin();
    void loop();
    void publish(const char* topic, const char* message);
    
private:
    void reconnect();

    const char* server;
    uint16_t port;

    WiFiClient espClient;
    PubSubClient client;

    unsigned long lastReconnectAttempt = 0;
    const unsigned long reconnectInterval = 1000;
};

#endif
