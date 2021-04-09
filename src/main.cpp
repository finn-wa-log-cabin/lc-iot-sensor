#include "main.h"

DHT_Unified dht(DHTPIN, DHT11);
WiFiClient net;
MQTTClient client;
unsigned long previousMillis = 0;

sensors_event_t temperature;
sensors_event_t humidity;
char timestamp[16] = {0};
int messageCount = 0;

void initWiFiManager() {
  WiFiManager wifiManager;
  wifiManager.autoConnect("IOT Sensor");
  Serial.print("Confirming WiFi connection...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" Done.");
}

void initNtp() {
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
 * Creates a millisecond-based Unix timestamp stored in a string to avoid 64-bit
 * calculations
 */
void updateTimestamp() { sprintf(timestamp, "%ld%03d", now(), ms()); }

void serialize() {
  StaticJsonDocument<192> doc;
  doc["version"] = VERSION;
  doc["customerID"] = CUSTOMER_ID;
  doc["deviceID"] = DEVICE_ID;
  doc["messageCount"] = messageCount;
  JsonObject sensorData = doc.createNestedObject("sensorData");
  sensorData["humidity"] = humidity.relative_humidity;
  sensorData["temperature"] = temperature.temperature;
  sensorData["timestamp"] = timestamp;
  char buffer[192];
  size_t n = serializeJson(doc, buffer);
  // TODO: If I send the real buffer, it disconnects
  client.publish("hellochen", "buffer"); // n);
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.print("\nconnecting...");
  while (!client.connect("arduino", "public", "public")) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");
  client.subscribe("/hellochen");
}

void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
}

void initMqtt() {
  client.begin("public.cloud.shiftr.io", net);
  client.onMessage(messageReceived);
  connect();
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  initWiFiManager();
  initNtp();
  initMqtt();
}

void loop() {
  events();
  client.loop();
  delay(10); // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

  if (millis() - previousMillis >= INTERVAL) {
    previousMillis = millis();
    messageCount++;
    Serial.println(messageCount);

    updateReadings();
    updateTimestamp();
    serialize();
  }
}