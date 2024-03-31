#include "ESP8266WiFi.h"
#include <PubSubClient.h>
#include "config.h"

#define LATCH_PIN 4
#define R1_PIN 5
#define R2_PIN 12
bool retain = true;

const int analogInPin = A0;
int bat_percentage;
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
  WiFi.config(DATA_IPADDR, DATA_GATEWY, DATA_IPMASK);
  uint8_t my_bssid[] = DATA_WIFI_BSSID;
  char my_ssid[] = WIFI_SSID;
  char my_auth[] = WIFI_AUTH;
  WiFi.begin(&my_ssid[0], &my_auth[0], DATA_WIFI_CH, &my_bssid[0], true);
  uint32_t timeout = millis() + 5000;
  while ((WiFi.status() != WL_CONNECTED) && (millis()<timeout)) { delay(5); }
  return (WiFi.status() == WL_CONNECTED);  
}




void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the R1 if an 1 was received as first character
  if ((char)payload[0] == '1') {
      digitalWrite(R1_PIN, HIGH); // Turn on the R1
      R1_startMillis = millis();
      R1_oneshot ++;

  } else {
    digitalWrite(R1_PIN, LOW);  // // Turn off the R1
  }



  // Switch on the R2 if an 1 was received as second character
  if ((char)payload[1] == '1') {
      digitalWrite(R2_PIN, HIGH);   // Turn on the R2
      R2_startMillis = millis();
      R2_oneshot ++;
    

  } else {
    digitalWrite(R2_PIN, LOW);  // Turn off the R2
  }

}




void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if(client.setBufferSize(380)){
    Serial.println("Buffer Size increased to 380 byte"); 
    }else{
     Serial.println("Failed to allocate larger buffer");   
     }
    // Create a random client ID
    String clientId = "ESPBell-Max-";
    // Attempt to connect
    if (client.connect(clientId.c_str(),mqtt_username, mqtt_password)) {
        Serial.println("connected");
        // Once connected, publish an announcement...
  
        // Send auto-discovery message for doorbell, //device_class - I tried using switch but with it auto-discovery not working
        client.publish(
          "homeassistant/binary_sensor/espbell-max/state/config", 
          "{\"name\":\"Bell\",\"unit_of_measurement\":\"\",\"device_class\":\"door\",\"icon\":\"mdi:bell\",\"state_topic\":\"homeassistant/binary_sensor/espbell-max/state\",\"unique_id\":\"espbell-max_bell\",\"payload_on\":\"on\",\"payload_off\":\"off\",\"device\":{\"identifiers\":[\"espbell-max\"],\"name\":\"ESPBell-MAX\",\"mdl\":\"ESPBell-MAX\",\"mf\":\"PricelessToolkit\"}}",
          retain
        );

        // Send auto-discovery message for RSSI
        client.publish(
          "homeassistant/sensor/espbell-max/rssi/config",
          "{\"name\":\"RSSI\",\"unit_of_measurement\":\"dBm\",\"device_class\":\"signal_strength\",\"icon\":\"mdi:signal\",\"entity_category\":\"diagnostic\",\"state_topic\":\"homeassistant/sensor/espbell-max/rssi\",\"unique_id\":\"espbell-max_signal\",\"device\":{\"identifiers\":[\"espbell-max\"],\"name\":\"ESPBell-MAX\"}}",
          retain
        );

        // Send auto-discovery message for Battery %
        client.publish(
          "homeassistant/sensor/espbell-max/batt/config",
          "{\"name\":\"Battery\",\"unit_of_measurement\":\"%\",\"device_class\":\"battery\",\"icon\":\"mdi:battery\",\"entity_category\":\"diagnostic\",\"state_topic\":\"homeassistant/sensor/espbell-max/batt\",\"unique_id\":\"espbell-max_batt\",\"device\":{\"identifiers\":[\"espbell-max\"],\"name\":\"ESPBell-MAX\"}}",
          retain
        );

        // Send DoorBell Status "on"
        client.publish("homeassistant/binary_sensor/espbell-max/state", "on");
        

        
        // Reading and publishing RSSI Value
        String rs = String(WiFi.RSSI());
        client.publish("homeassistant/sensor/espbell-max/rssi", rs.c_str(), retain);




        int batt = map(analogRead(A0), 320, 420, 0, 100); // (analogRead(A2), MIN_ADC, MAX_ADC, 0, 100);
        batt = constrain(batt, 0, 100); // Constrain the value between 0 and 100

        client.publish("homeassistant/sensor/espbell-max/batt", String(batt).c_str(), retain);

        // Subscribing to Open command "R1/R2"
        client.subscribe("homeassistant/sensor/espbell-max/command");
     } else {
       Serial.print("failed, rc=");
       Serial.print(client.state());
       Serial.println(" try again in 0.05 seconds");
       // Wait 0.05 seconds before retrying
       delay(50);
    }
  }
}




void setup() {
  WiFi.setAutoConnect(false); // prevent early autoconnect
  pinMode(LATCH_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, HIGH);
  pinMode(R1_PIN, OUTPUT);
  pinMode(R2_PIN, OUTPUT);

  //pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  uint32_t ts_start = millis();
  int wifi_fast_ok = wifi_fast();
  //delay(500);
  //Serial.print("Timing wifi_fast(): ");
  //Serial.print(millis()-ts_start); Serial.println("ms");
  //Serial.print("Result="); Serial.println(wifi_fast_ok);

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  startMillis = millis();
  
}




void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();


  
  if(R1_oneshot >= 1){
    R1_currentMillis = millis();
    if (R1_currentMillis - R1_startMillis >= openTime){
      digitalWrite(R1_PIN, LOW); // Turn off the R1
      R1_oneshot = 0;
      if (autoOff == 1){
        client.publish("homeassistant/binary_sensor/espbell-max/state", "off", retain);
        delay(10);
        client.endPublish();
        digitalWrite(LATCH_PIN, LOW);
      }
    }
  }

  if(R2_oneshot >= 1){
    R2_currentMillis = millis();
    if (R2_currentMillis - R2_startMillis >= openTime){
      digitalWrite(R2_PIN, LOW); // Turn off the R2
      R2_oneshot = 0;
      if (autoOff == 1){
        client.publish("homeassistant/binary_sensor/espbell-max/state", "off", retain);
        delay(10);
        client.endPublish();
        digitalWrite(LATCH_PIN, LOW);
      }
    }
  }


  unsigned long currentMillis = millis();
  if (currentMillis + startMillis >= upTime) {
    // Reset doorbell status and command before poweroff
    client.publish("homeassistant/binary_sensor/espbell-max/state", "off", retain);
    delay(15);
    client.publish("homeassistant/sensor/espbell-max/command", "00", retain);
    delay(15);
    client.endPublish();
	delay(15);
    digitalWrite(LATCH_PIN, LOW);
    }
}
