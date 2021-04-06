const unsigned long SECOND = 1000UL;
const unsigned long MINUTE = 60UL * SECOND;
const unsigned long HOUR = 60UL * MINUTE;

// Interval time(ms) for sending message to IoT Hub
const unsigned long INTERVAL = 30UL * SECOND;

// Telemetry message interface version
const char *VERSION = "1.0.0";

// Physical device information for board and sensor
const char *CUSTOMER_ID = "WA";
const char *DEVICE_ID = "FeatherHuzzah1";

// Digital pin connected to the DHT sensor
#define DHTPIN 2