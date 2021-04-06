#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
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
const unsigned long INTERVAL = 5UL * TIME_SECOND;
unsigned long previousMillis = 0;

char jsonStr[256] = {0};
sensors_event_t temperature;
sensors_event_t humidity;
char timestamp[16] = {0};
int msgCount = 0;

void setup() {
  Serial.begin(115200);
  dht.begin();

  WiFiManager wifiManager;
  wifiManager.autoConnect("IOT Sensor");

  // Wait for NTP server to connect for the first time
  setDebug(INFO);
  waitForSync();
  // Only sync NTP manually from now on
  // setInterval();
}

void updateReadings() {
  dht.temperature().getEvent(&temperature);
  if (isnan(temperature.temperature)) {
    Serial.println("Error reading temperature!");
  }
  dht.humidity().getEvent(&humidity);
  if (isnan(humidity.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
}

/**
 * Creates a millisecond-based Unix timestamp stored in a string to avoid
 * 64-bit calculations
 */
void updateTimestamp() { sprintf(timestamp, "%ld%03d", now(), ms()); }

void serialize() {
  StaticJsonDocument<256> doc;
  doc["version"] = "1.0.0";
  doc["customerID"] = "TestCustomer";
  doc["deviceID"] = "TestDevice1";
  JsonObject sensorData = doc.createNestedObject("sensorData");
  doc["messageCount"] = msgCount;
  sensorData["humidity"] = humidity.relative_humidity;
  sensorData["temperature"] = temperature.temperature;
  sensorData["timestamp"] = timestamp;
  serializeJson(doc, jsonStr);
  Serial.println(jsonStr);
}

void loop() {
  events();
  if (millis() - previousMillis >= INTERVAL) {
    previousMillis = millis();
    msgCount++;
    updateReadings();
    updateTimestamp();
    serialize();
  }
}