#include "secrets.h"

const unsigned long SECOND = 1000UL;
const unsigned long MINUTE = 60UL * SECOND;
const unsigned long HOUR = 60UL * MINUTE;

// Interval time(ms) for sending message to IoT Hub
const unsigned long INTERVAL = 1UL * MINUTE;

// Telemetry message interface version
const char *VERSION = "1.0.0";

// Physical device information for board and sensor
const char *CUSTOMER_ID = "WA";
const char *DEVICE_ID = "FeatherHuzzah1";

// Digital pin connected to the DHT sensor
#define DHTPIN 2

// CONNECTION information to complete
#define IOTHUBNAME "WaterTankMonitors"
#define DEVICEID "FeatherHuzzah1"

// computed Host Username and Topic
#define USERNAME                                                               \
  IOTHUBNAME ".azure-devices.net/" DEVICEID                                    \
             "/?api-version=2020-09-30&DeviceClientType=c%2F1.2.0-beta.1"
#define PORT 8883
#define HOST IOTHUBNAME ".azure-devices.net"
#define TOPIC "devices/" DEVICEID "/messages/events/"
