// WiFi Configuration
#define WIFI_SSID "Your_WIFI_SSID"
#define WIFI_AUTH "Your_WIFI_Password"
#define DATA_WIFI_CH 6
#define DATA_WIFI_BSSID {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA} // Change the AA values as needed

// Network Settings: IP address, gateway, mask, DNS
#define DATA_IPADDR IPAddress(192,168,99,2) // Use an IP address outside the DHCP range
#define DATA_IPMASK IPAddress(255,255,255,0)
#define DATA_GATEWAY IPAddress(192,168,99,1)
#define DATA_DNS1   IPAddress(192,168,99,1)

// MQTT Credentials
const char* mqtt_username = "Your_MQTT_Username";
const char* mqtt_password = "Your_MQTT_Password";
const char* mqtt_server   = "MQTT/HomeAssistant_IP";
const int mqtt_port       = 1883;

// Timing settings (in milliseconds)
// upTime: Duration to wait after wakeup before powering off (Latch PIN to LOW)
const long upTime = 15000;
// openTime: Duration that the relay remains on
const long openTime = 2000;
// autoOff: 0 = DISABLE / 1 = ENABLE auto shutdown after openTime
const int autoOff = 1;

// Connection timeouts (in milliseconds)
// If the board cannot connect to WiFi within TimeToConnectWIFI, it powers off.
const long TimeToConnectWIFI = 30000;
// If the board cannot connect to the MQTT server within TimeToConnectMQTT, it powers off.
const long TimeToConnectMQTT = 40000;

// MQTT Command Topic: "homeassistant/sensor/espbell-max/command"
// Payload format (2 characters): first for R1, second for R2
// '1' = ON, '0' = OFF. Example: "11" turns both on, "10" turns R1 on and R2 off.
