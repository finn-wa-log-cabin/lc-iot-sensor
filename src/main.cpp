#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ezTime.h>

#define ISO8601_MS "Y-m-d\\TH:i:s.vO"

#define DHTPIN 2 // Digital pin connected to the DHT sensor
DHT_Unified dht(DHTPIN, DHT11);

const unsigned long TIME_SECOND = 1000UL;
const unsigned long TIME_MINUTE = 60UL * TIME_SECOND;
const unsigned long TIME_HOUR = 60UL * TIME_MINUTE;
const unsigned long INTERVAL = 10UL * TIME_SECOND;

unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFiManager wifiManager;
  wifiManager.autoConnect("IOT Sensor");

  // Wait for NTP server to connect for the first time
  waitForSync();
  // Only sync NTP manually from now on
  setInterval();
}

void printReadings() {
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  } else {
    Serial.printf("Temperature: %.2f\n", event.temperature);
  }
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  } else {
    Serial.printf("Humidity: %.0f\n", event.relative_humidity);
  }
}

void loop() {
  if (millis() - previousMillis >= INTERVAL) {
    updateNTP();
    previousMillis = millis();
    Serial.println(dateTime(now(), ISO8601_MS));
    printReadings();
  }
}