<img src="https://raw.githubusercontent.com/PricelessToolkit/ESPBell-MAX/main/img/banner.jpg"/>

🤗 If you value the effort I've put into crafting this open-source project and would like to show your support, consider subscribing to my [YouTube channel](https://www.youtube.com/@PricelessToolkit/videos). Your subscription goes a long way in backing my work.
# ESPBell-MAX and [ESPBell-LITEl](https://github.com/PricelessToolkit/ESPBell-LITE)
ESPBell-MAX "IoT Intercom / Doorbell" module specifically made for Home Assistant, enables you to stay informed whenever someone rings your doorbell or presses the intercom button. Upon receiving an interactive notification, you have the convenience to either promptly open the door or choose to dismiss the alert by simply tapping the button within the notification.

### Links

- YouTube video [https://youtu.be/gf1WWKyEnbg](https://www.youtube.com/@PricelessToolkit/videos)
- My Shop
  - ESPBell-MAX [https://www.pricelesstoolkit.com/](https://www.pricelesstoolkit.com)
  - UNIProg Programmer [https://www.pricelesstoolkit.com/](https://www.pricelesstoolkit.com/en/projects/33-uniprog-uartupdi-programmer-33v.html)

## Specifications
- Based on ESP12F
- Power consumption
  - Standby mode: 0uAh
  - When triggered: 21uAs - a few seconds
- Input / Output
  - 2 x "R1/R2" SSR 2.4A 30v Max
  - 1 x "DB" Input Optocoupler 2-30v Max "For Doorbell"
  - 1 x "ET" Input 3.3-5v Max "External Trigger"
  - 1 x "B+" Battery Voltage "Permanent output"
  - 1 x "E+" Battery Voltage "When ESPBell is enabled"
  - 1 x GPIO14
  - 2 x GND "Only! for ET/B+/E+/GPIO14"
- Status LEDs - Can be deactivated "PCB Jumper"
  - 1 x R1
  - 1 x R2
  - 1 x DoorBell
  - 1x Power
  - 1x On ESP module "Not used"
- Battery type Li-Ion 18650
- Button for flashing Firmware

## Ready-made KIT
- Assembled PCB
- Cable with Connector 12P 30cm
- Battery "Not Included"
- 3D Printed Case "Optional"

## ESPBell-MAX Code Configuration
### Used Arduino Libraries
```c
#include "ESP8266WiFi.h"
#include <PubSubClient.h>
```
### All settings are done in the file "config.h"
```c
#define WIFI_SSID "Your_WIFI_SSID"
#define WIFI_AUTH "Your_WIFI_Password"
#define DATA_WIFI_CH 6
#define DATA_WIFI_BSSID {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA} // Change only AA part.
```
```c
// IP address, network mask, Gateway, DNS
#define DATA_IPADDR IPAddress(192,168,99,2) // Use IP address outside of DHCP Range 
#define DATA_IPMASK IPAddress(255,255,255,0)
#define DATA_GATEWY IPAddress(192,168,99,1)
#define DATA_DNS1   IPAddress(192,168,99,1)
```
```c
const char* mqtt_username = "Your_MQTT_Username";
const char* mqtt_password = "Your_MQTT_Password";
const char* mqtt_server = "MQTT/HomeAssistant_IP";
const int mqtt_port = 1883;
```
```c
// 1000 = 1s"
const long upTime = 15000;  // How long to wait after wakeup before powering off "Latch PIN to LOW" 
const long openTime = 2000;  // How long relays is on.
int autoOff = 1; // 0 = DISABLE / 1 = ENABLE : Powerig off automaticaly after "openTime" : if is set to 0 ESPBell will power down after upTime.
```

### Solid-state relays are controlled by publishing MQTT payload
MQTT Command Topic is "homeassistant/sensor/espbell-max/command" Payload is "00" firs digit is controlling R1 second digit R2

* "01" R1=OFF R2=ON
* "11" R1=ON R2=ON
* "10" R1=ON R2=OFF
* "00" R1=OFF R2=OFF


## Home Assistant Configuration
Here is a multi-user configuration, which means that a message is sent to several family members. If one family member clicks on the notification, the notification disappears from the other phones. We need to create 3 automations.

### Automation 1 "Intercom DoorBell Notification"
This automation sends an interactive notification to phones with the "tag: intercom"
```
alias: 🔔 Intercom DoorBell Notification
description: ""
trigger:
  - platform: state
    entity_id:
      - binary_sensor.espbell_max_bell
    to: "on"
condition: []
action:
  - service: notify.mobile_app_doogee_v20pro
    data:
      message: Someone at the door
      data:
        persistent: true
        importance: high
        channel: intercom
        tag: intercom
        image: /media/local/notify/doorbell.jpg
        actions:
          - action: intercom_ignore
            title: Ignore ✖
          - action: intercom_open
            title: Open The Door 🔓
      title: DoorBell 🔔
      message: Quelqu'un à la porte
  - service: notify.mobile_app_Second_Phone
    data:
      message: Someone at the door
      data:
        persistent: true
        importance: high
        channel: intercom
        tag: intercom
        image: /media/local/notify/doorbell.jpg
        actions:
          - action: intercom_ignore
            title: Ignore ✖
          - action: intercom_open
            title: Open The Door 🔓
      title: DoorBell 🔔
      message: Quelqu'un à la porte
mode: single

```

### Automation 2 "Intercom DoorBell ignore notification 'Dismiss'"
This automation clears notifications on phones when a button on the notification is pressed.
```
alias: 🔔 Intercom DoorBell ignore notification Dismiss
description: ""
trigger:
  - platform: event
    event_data:
      action: intercom_ignore
    event_type: mobile_app_notification_action
condition: []
action:
  - service: notify.mobile_app_doogee_v20pro
    data:
      message: clear_notification
      data:
        tag: intercom
  - service: notify.mobile_app_Second_Phone
    data:
      message: clear_notification
      data:
        tag: intercom
mode: single
```

### Automation 3 "Intercom open the door and Dismiss notification"
```
alias: 🔔 Intercom open the door and Dismiss notification
description: ""
trigger:
  - platform: event
    event_data:
      action: intercom_open
    event_type: mobile_app_notification_action
condition: []
action:
  - service: mqtt.publish
    data:
      qos: 0
      retain: false
      topic: homeassistant/sensor/espbell-max/command
      payload: "10"
  - service: notify.mobile_app_doogee_v20pro
    data:
      message: clear_notification
      data:
        tag: intercom
  - service: notify.mobile_app_Second_Phone
    data:
      message: clear_notification
      data:
        tag: intercom
mode: single

```
