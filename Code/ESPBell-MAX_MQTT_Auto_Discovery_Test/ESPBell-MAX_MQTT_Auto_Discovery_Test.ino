#include "ESP8266WiFi.h"
#include <PubSubClient.h>
#include "config.h"

#define LATCH_PIN 4
#define R1_PIN 5
#define R2_PIN 12
bool retain = true;

unsigned long startMillis = 0;

unsigned long R1_startMillis;
unsigned long R1_currentMillis;
unsigned long R2_startMillis;
unsigned long R2_currentMillis;
int R1_oneshot = 0;
int R2_oneshot = 0;

WiFiClient espClient;
PubSubClient client(espClient);

int wifi_fast() {
  WiFi.persistent(true); 
  WiFi.mode(WIFI_STA);
  WiFi.config(DATA_IPADDR, DATA_GATEWAY, DATA_IPMASK);
  uint8_t my_bssid[] = DATA_WIFI_BSSID;
  char my_ssid[] = WIFI_SSID;
  char my_auth[] = WIFI_AUTH;
  WiFi.begin(&my_ssid[0], &my_auth[0], DATA_WIFI_CH, &my_bssid[0], true);
  uint32_t timeout = millis() + TimeToConnectWIFI;
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() >= timeout) {
      Serial.println("Failed to connect to WiFi, powering off.");
      digitalWrite(LATCH_PIN, LOW);  // Trigger shutdown; board will power off immediately.
    }
    delay(5);
  }
  return (WiFi.status() == WL_CONNECTED);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Check that payload has at least one character for R1
  if (length >= 1 && (char)payload[0] == '1') {
    digitalWrite(R1_PIN, HIGH); // Turn on R1
    R1_startMillis = millis();
    R1_oneshot++;
  } else {
    digitalWrite(R1_PIN, LOW);  // Turn off R1
  }

  // Check that payload has at least two characters for R2
  if (length >= 2 && (char)payload[1] == '1') {
    digitalWrite(R2_PIN, HIGH); // Turn on R2
    R2_startMillis = millis();
    R2_oneshot++;
  } else {
    digitalWrite(R2_PIN, LOW);  // Turn off R2
  }
}

// Function to read the average of 20 ADC values
int getAverageADCReading() {
  int total = 0;
  for (int i = 0; i < 20; i++) {
    total += analogRead(A0);
  }
  return total / 20;
}

void reconnect() {
  unsigned long reconnectStart = millis();
  // Loop until we're reconnected or the timeout is reached
  while (!client.connected()) {
    if (millis() - reconnectStart >= TimeToConnectMQTT) {
      Serial.println("Failed to connect to MQTT server, powering off.");
      digitalWrite(LATCH_PIN, LOW);
      return;
    }
    
    Serial.print("Attempting MQTT connection...");
    if (client.setBufferSize(380)) {
      Serial.println("Buffer Size increased to 380 byte");
    } else {
      Serial.println("Failed to allocate larger buffer");
    }
    // Create a random client ID
    String clientId = "ESPBell-Max-";
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Auto-discovery messages for Home Assistant
      client.publish(
        "homeassistant/binary_sensor/espbell-max/state/config", 
        "{\"name\":\"Bell\",\"unit_of_measurement\":\"\",\"device_class\":\"door\",\"icon\":\"mdi:bell\",\"state_topic\":\"homeassistant/binary_sensor/espbell-max/state\",\"unique_id\":\"espbell-max_bell\",\"payload_on\":\"on\",\"payload_off\":\"off\",\"device\":{\"identifiers\":[\"espbell-max\"],\"name\":\"ESPBell-MAX\",\"mdl\":\"ESPBell-MAX\",\"mf\":\"PricelessToolkit\"}}",
        retain
      );
      client.publish(
        "homeassistant/sensor/espbell-max/rssi/config",
        "{\"name\":\"RSSI\",\"unit_of_measurement\":\"dBm\",\"device_class\":\"signal_strength\",\"icon\":\"mdi:signal\",\"entity_category\":\"diagnostic\",\"state_topic\":\"homeassistant/sensor/espbell-max/rssi\",\"unique_id\":\"espbell-max_signal\",\"device\":{\"identifiers\":[\"espbell-max\"],\"name\":\"ESPBell-MAX\"}}",
        retain
      );
      client.publish(
        "homeassistant/sensor/espbell-max/batt/config",
        "{\"name\":\"Battery\",\"unit_of_measurement\":\"%\",\"device_class\":\"battery\",\"icon\":\"mdi:battery\",\"entity_category\":\"diagnostic\",\"state_topic\":\"homeassistant/sensor/espbell-max/batt\",\"unique_id\":\"espbell-max_batt\",\"device\":{\"identifiers\":[\"espbell-max\"],\"name\":\"ESPBell-MAX\"}}",
        retain
      );
      // Publish initial doorbell status, RSSI, and battery level
      client.publish("homeassistant/binary_sensor/espbell-max/state", "on");
      
      String rs = String(WiFi.RSSI());
      client.publish("homeassistant/sensor/espbell-max/rssi", rs.c_str(), retain);
      
      int average = getAverageADCReading();
      int batteryPercentage = map(average, 670, 880, 0, 100);  
      int batteryP = constrain(batteryPercentage, 0, 100);
      client.publish("homeassistant/sensor/espbell-max/batt", String(batteryP).c_str(), retain);
      
      // Subscribe to the command topic
      client.subscribe("homeassistant/sensor/espbell-max/command");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 0.05 seconds");
      delay(50);
    }
  }
}

void setup() {
  WiFi.setAutoConnect(false); // Prevent early autoconnect
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, HIGH);
  pinMode(R1_PIN, OUTPUT);
  pinMode(R2_PIN, OUTPUT);

  Serial.begin(115200);
  int wifi_fast_ok = wifi_fast();
  Serial.print("WiFi connection status: ");
  Serial.println(wifi_fast_ok);
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  startMillis = millis();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  if (R1_oneshot >= 1) {
    R1_currentMillis = millis();
    if (R1_currentMillis - R1_startMillis >= openTime) {
      digitalWrite(R1_PIN, LOW);
      R1_oneshot = 0;
      if (autoOff == 1) {
        client.publish("homeassistant/binary_sensor/espbell-max/state", "off", retain);
        Serial.println("R1 Opened - now Powering OFF");
        delay(20);
        digitalWrite(LATCH_PIN, LOW);
      }
    }
  }

  if (R2_oneshot >= 1) {
    R2_currentMillis = millis();
    if (R2_currentMillis - R2_startMillis >= openTime) {
      digitalWrite(R2_PIN, LOW);
      R2_oneshot = 0;
      if (autoOff == 1) {
        client.publish("homeassistant/binary_sensor/espbell-max/state", "off", retain);
        Serial.println("R2 Opened - now Powering OFF");
        delay(20);
        digitalWrite(LATCH_PIN, LOW);
      }
    }
  }

  unsigned long currentMillis = millis();
  // Correct uptime check using subtraction of startMillis
  if (currentMillis - startMillis >= upTime) {
    client.publish("homeassistant/binary_sensor/espbell-max/state", "off", retain);
    Serial.println("Reseting Bell State to off");
    delay(20);
    client.publish("homeassistant/sensor/espbell-max/command", "00", retain);
    Serial.println("Reseting Open Command to 00");
    delay(40);
    Serial.println("UpTime reached. Powering OFF.");
    digitalWrite(LATCH_PIN, LOW);
  }
}
