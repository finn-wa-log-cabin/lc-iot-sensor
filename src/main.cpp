#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ezTime.h>

#define ISO8601_MS "Y-m-d\\TH:i:s.vO"

const unsigned long TIME_SECOND = 1000UL;
const unsigned long TIME_MINUTE = 60UL * TIME_SECOND;
const unsigned long TIME_HOUR = 60UL * TIME_MINUTE;
const unsigned long INTERVAL = 10UL * TIME_SECOND;

unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  WiFiManager wifiManager;
  wifiManager.autoConnect("IOT Sensor");

  // Wait for NTP server to connect for the first time
  waitForSync();
  // Only sync NTP manually from now on
  setInterval();
}

void loop() {
  if (millis() - previousMillis >= INTERVAL) {
    updateNTP();
    previousMillis = millis();
    Serial.println(dateTime(now(), ISO8601_MS));
  }
}