#include "SystemController.h"

const char* ssid = "Forger Hideout";
const char* password = "AnyaLoidYor";
const char* mqtt_server = "192.168.68.60";

SystemController controller(ssid, password, mqtt_server);

void setup() {
  controller.begin();
}

void loop() {
  controller.loop();
}