// data from your Wifi connect: SSID, Password, Channel, BSSID"MAC"
#define WIFI_SSID "Your_WIFI_SSID"
#define WIFI_AUTH "Your_WIFI_Password"
#define DATA_WIFI_CH 6
#define DATA_WIFI_BSSID {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA} // Change only AA part.


// IP address, network mask, Gateway, DNS
#define DATA_IPADDR IPAddress(192,168,99,2) // Use IP address outside of DHCP Range 
#define DATA_IPMASK IPAddress(255,255,255,0)
#define DATA_GATEWY IPAddress(192,168,99,1)
#define DATA_DNS1   IPAddress(192,168,99,1)


const char* mqtt_username = "Your_MQTT_Username";
const char* mqtt_password = "Your_MQTT_Password";
const char* mqtt_server = "MQTT/HomeAssistant_IP";
const int mqtt_port = 1883;


// 1000 = 1s"
const long upTime = 15000;  // How long to wait after wakeup before powering off "Latch PIN to LOW" 
const long openTime = 2000;  // How long relays is on.
int autoOff = 1; // 0 = DISABLE / 1 = ENABLE : Powerig off automaticaly after "openTime" : if is set to 0 ESPBell will power down after upTime.




// MQTT Command Topic is "homeassistant/sensor/espbell-max/command"
// Payload is "00" firs digit is controlling R1 second digit R2
// 0 = OFF / 1 = ON
// "01" R1=OFF R2=ON
// "11" R1=ON R2=ON
// "10" R1=ON R2=OFF
// "00" R1=OFF R2=OFF
