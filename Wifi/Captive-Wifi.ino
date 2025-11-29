// Libraries to download
// Libraries:
// ESP8266WiFi.h
// DNSServer.h
// ESP8266WebServer.h
// Add in Manager URLs
// http://arduino.esp8266.com/stable/package_esp8266com_index.json



#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

const byte DNS_PORT = 53;
DNSServer dnsServer;
ESP8266WebServer webServer(80);

// Access Point credentials
const char *ap_ssid = "MyHotspot";      
const char *ap_password = "12345678";   

// Your existing Wi-Fi (for internet)
const char *sta_ssid = "HomeWiFi";      
const char *sta_password = "YourPassword";   

void handleRoot() {
  webServer.send(200, "text/html",
  "<meta http-equiv='refresh' content='0;url=https://yourwebsite.com'>"
  "<h2>Connecting...</h2>");
}

void setup() {
  Serial.begin(115200);

  // Start in both AP + STA mode
  WiFi.mode(WIFI_AP_STA);

  // Connect to your home Wi-Fi (STA)
  WiFi.begin(sta_ssid, sta_password);
  Serial.print("Connecting to home Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to home Wi-Fi!");
  Serial.print("Home IP: ");
  Serial.println(WiFi.localIP());

  // Start Access Point
  WiFi.softAP(ap_ssid, ap_password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // DNS setup (redirect all domains to ESP)
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());

  // Captive portal page
  webServer.on("/", handleRoot);
  webServer.onNotFound(handleRoot);
  webServer.begin();

  Serial.println("Dual-mode captive portal started!");
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
}