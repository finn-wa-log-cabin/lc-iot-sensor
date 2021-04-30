#include "main.h"

DHT_Unified dht(DHTPIN, DHT11);
WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);
unsigned long previousMillis = 0;

sensors_event_t temperature;
sensors_event_t humidity;
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

void serialize() {
  StaticJsonDocument<192> doc;
  doc["version"] = VERSION;
  doc["customerID"] = CUSTOMER_ID;
  doc["deviceID"] = DEVICE_ID;
  doc["messageCount"] = messageCount;
  JsonObject sensorData = doc.createNestedObject("sensorData");
  sensorData["humidity"] = humidity.relative_humidity;
  sensorData["temperature"] = temperature.temperature;
  sensorData["timestamp"] = now();
  char buffer[192];
  size_t n = serializeJson(doc, buffer);
  mqttClient.publish(TOPIC, buffer, n);
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // This esp8266 var is used to verify certificate validity
    // but it is not set by ezTime so we set it manually
    sntp_real_timestamp = now();
    if (mqttClient.connect(DEVICE_ID, USERNAME, PWD)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void initMqtt() {
  if (!wifiClient.setCACert((const uint8_t *)ca_pem, ca_pem_len)) {
    Serial.println("setCACert() FAILED");
    return;
  }
  mqttClient.setServer(HOST, PORT);
  mqttClient.setBufferSize(MSG_SIZE);
  connect();
}

void sendTelemetry() {
  Serial.println(messageCount);
  updateReadings();
  serialize();
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  initWiFiManager();
  initNtp();
  initMqtt();
  // Send first message
  sendTelemetry();
}

void loop() {
  events();
  if (!mqttClient.connected()) {
    connect();
  }
  mqttClient.loop();

  if (millis() - previousMillis >= INTERVAL) {
    previousMillis = millis();
    messageCount++;
    sendTelemetry();
  }
}