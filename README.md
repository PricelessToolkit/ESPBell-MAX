<img src="https://raw.githubusercontent.com/PricelessToolkit/ESPBell-MAX/main/img/banner.jpg"/>

🤗 If you value the effort I've put into crafting this open-source project and would like to show your support, consider subscribing to my [YouTube channel](https://www.youtube.com/@PricelessToolkit/videos). Your subscription goes a long way in backing my work.
# ESPBell-MAX and [ESPBell-LITE](https://github.com/PricelessToolkit/ESPBell-LITE)
ESPBell-MAX "IoT Intercom / Doorbell" module specifically made for Home Assistant, enables you to stay informed whenever someone rings your doorbell or presses the intercom button. Upon receiving an interactive notification, you have the convenience to either promptly open the door or choose to dismiss the alert by simply tapping the button within the notification. See the screenshot below.
<details>
  <summary>Notification Screenshot</summary>
<img src="https://raw.githubusercontent.com/PricelessToolkit/ESPBell-MAX/main/img/notification_m.jpg"/>
</details>

### Links

- YouTube video [https://youtu.be/gf1WWKyEnbg](https://www.youtube.com/@PricelessToolkit/videos)
- My Shop
  - ESPBell-MAX [https://www.pricelesstoolkit.com/](https://www.pricelesstoolkit.com)
  - UNIProg Programmer [https://www.pricelesstoolkit.com/](https://www.pricelesstoolkit.com/en/projects/33-uniprog-uartupdi-programmer-33v.html)

## Specifications
- Based on ESP12F "Programmable via pads on the PCB"
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

## Before you start
> [!NOTE]
> I trust that everything is crystal clear as I've made every effort to provide a step-by-step explanation on the setup process. If you have any further questions regarding the setup or require assistance with assembling a printed circuit board, feel free to open a new discussion topic in the dedicated tab.

> [!IMPORTANT]
> If you're new to Arduino-related matters, please refrain from asking how to install the Arduino IDE. There are already plenty of excellent tutorials available on the internet. And if you encounter any issues, remember that providing detailed information about the problem will help me offer more effective assistance. More information equals better help!



## PCB Assembly
This project is open-source, allowing you to assemble ESPBell-MAX on your own. To simplify this process, I've provided an "Interactive HTML Boom File" located in the PCB folder. This interactive file helps you identify where to solder each component and polarity, reducing the chances of errors to a minimum. But if you don't feel confident in assembling it yourself, you can always opt to purchase a pre-assembled board from my [Shop](https://www.pricelesstoolkit.com)

## Arduino IDE Configuration
### Used Arduino Libraries
```c
#include "ESP8266WiFi.h"
#include <PubSubClient.h>
```

### If you are using ESP8266 for the first time, you need To install the ESP8266 board and PubSubClient library in your Arduino IDE.
- In your Arduino IDE, go to File> Preferences.
- Enter http://arduino.esp8266.com/stable/package_esp8266com_index.json into the “Additional Boards Manager URLs” field. Then, click the “OK” button
- Open the Boards Manager. Go to Tools > Board > Boards Manager.
- Search for ESP8266 and press the install button for the “ESP8266 by ESP8266 Community“
- Search for PubSubClient and press the install button.


For board configuration, see the screenshot below
<details>
  <summary>Board config Screenshot</summary>
<img src="https://raw.githubusercontent.com/PricelessToolkit/ESPBell-MAX/main/img/arduino.jpg"/>
</details>

## ESPBell-MAX Code Configuration
> [!NOTE]  
> All configurations are done in the file "config.h"

### WI-FI Settings
To ensure a rapid Wi-Fi connection, we skip the Wi-Fi scanning process. Instead, we simply provide the access point's name, channel, and MAC address. This efficient method significantly shortens the connection time to the access point, taking just about 300 milliseconds.

> [!IMPORTANT]
> Make sure to set a fixed channel on your Wi-Fi access point.

```c
#define WIFI_SSID "Your_WIFI_SSID"
#define WIFI_AUTH "Your_WIFI_Password"
#define DATA_WIFI_CH 6 // Put your Wi-Fi channel and disable channel hopping in your WI-FI Access Point
#define DATA_WIFI_BSSID {0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA} // Change only AA part.
```
### Local Network Settings
As the DHCP-based IP address assignment may introduce certain delays, we can optimize the process by manually assigning and registering the IP address.
> [!IMPORTANT]
> Use IP address outside of DHCP Range.

```c
// IP address, network mask, Gateway, DNS
#define DATA_IPADDR IPAddress(192,168,99,2) // Use IP address outside of DHCP Range 
#define DATA_IPMASK IPAddress(255,255,255,0)
#define DATA_GATEWY IPAddress(192,168,99,1)
#define DATA_DNS1   IPAddress(192,168,99,1)
```
### MQTT Settings
Fill in Mosquitto broker credentials and IP address.

> [!IMPORTANT]
> SSL is disabled.

```c
const char* mqtt_username = "Your_MQTT_Username";
const char* mqtt_password = "Your_MQTT_Password";
const char* mqtt_server = "MQTT/HomeAssistant_IP";
const int mqtt_port = 1883;
```
### Timing Settings
Optimal performance and energy efficiency hinge upon these configurations, consisting of three critical parameters. It's important to note that 1000ms equates to 1 second.
<details>
  <summary>Explanation click here</summary>
upTime: This variable represents the time delay, in milliseconds, before initiating a 'Power Off' operation by setting the Latch PIN to LOW. You can adjust this value to determine how long the system should wait after waking up before powering down.

openTime: This variable sets the duration, in milliseconds, during which the relays remain active. Modify this value to control how long the relays stay on.

autoOff: This parameter can be set to either 0 or 1:
- 0: DISABLED - ESPBell will power down after the 'upTime' specified.
- 1: ENABLED - ESPBell will automatically power down after the 'openTime' has elapsed.
</details>

> [!IMPORTANT]
> Select your 'upTime' value carefully to ensure you have sufficient time for picking up your phone, checking notifications, and pressing the action button, along with a little extra time in reserve for added convenience.

```c
// 1000 = 1s"
const long upTime = 15000;  // How long to wait after wakeup before powering off "Latch PIN to LOW" 
const long openTime = 2000;  // How long relays is on.
int autoOff = 1; // 0 = DISABLE / 1 = ENABLE // Powering off automatically after "openTime" // if is set to 0 ESPBell will power down after upTime.
```


## Home Assistant Configuration
Here is a multi-user configuration, which means that a message is sent to several family members. If one family member clicks on the notification, the notification disappears from the other phones. For all this to work we need to create "three" automation, but before that, in this example, you need to change a minimum few things.
- Image file path that is used as a background for Notification.
- The name of the mobile device that is connected to the Home Assistant. In my case, it's "doogee_v20pro" and "Second_Phone"
  
```yaml
  image: /media/local/notify/doorbell.jpg
- service: notify.mobile_app_doogee_v20pro
- service: notify.mobile_app_Second_Phone

```


### Automation _1_
This automation sends an interactive notification with the "tag: intercom" to phones.

<details>
  <summary>Explanation click here</summary>
  Alias: This is a user-defined name or label for the automation. In this case, it's given the name "🔔 Intercom DoorBell Notification," which suggests that it's related to receiving notifications for an intercom or doorbell event.

Description: This field is left empty, so there's no additional description provided for this automation.

Trigger: The trigger specifies the event or condition that will start the automation. In this case, it is triggered when the state of the "binary_sensor.espbell_max_bell" changes to "on." This implies that the automation will run when the binary sensor named "espbell_max_bell" switches from an off state to an on state.

Condition: There are no additional conditions specified. This means the automation will proceed without any additional conditions beyond the trigger.

Action: The action section defines what should happen when the trigger condition is met. In this case, there are two actions defined:

The first action uses the "notify.mobile_app_doogee_v20pro" service to send a notification to a mobile device. The notification includes a message "Someone at the door" and is configured with various data attributes, including a persistent notification, high importance, a specified channel ("intercom"), and a tag ("intercom"). It also includes an image file path for the notification and two actions that can be taken by the recipient: "Ignore ✖" and "Open The Door 🔓." The title and message of the notification are also specified in two languages (English and French).

The second action is similar to the first but uses the "notify.mobile_app_Second_Phone" service to send the same notification to another mobile device.

Mode: The mode is set to "single," which means that the automation will only run once for each trigger event. Subsequent trigger events will not cause the automation to run again until the current execution has been completed.
</details>

```yaml
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

### Automation _2_
This automation clears notifications with the "tag: intercom" on phones when an ignore button on the notification is pressed.

<details>
  <summary>Explanation click here</summary>
Alias: The alias is a user-defined name or label for the automation, and in this case, it's named "🔔 Intercom DoorBell ignore notification Dismiss." This name suggests that the automation is related to dismissing or ignoring notifications for an intercom or doorbell event.

Description: The description field is left empty, so there's no additional description provided for this automation.

Trigger: This automation is triggered by an event. The trigger condition is defined as follows:

platform: The trigger platform is "event," which means the automation will be triggered by an event.

event_data: The automation will only trigger when the event data contains a specific action. In this case, the trigger event is looking for events with the action "intercom_ignore."

event_type: The automation is tied to the "mobile_app_notification_action" event type. It implies that this automation is designed to respond to actions taken by the user within a mobile app notification.

Condition: There are no additional conditions specified. This means the automation will proceed without any additional conditions beyond the trigger.

Action: The action section specifies what should happen when the trigger condition is met. This automation has two actions:

The first action uses the "notify.mobile_app_doogee_v20pro" service to send a notification with the message "clear_notification." It also includes data attributes with the "tag" set to "intercom." This effectively clears or dismisses the notification with the "intercom" tag on the "mobile_app_doogee_v20pro" mobile app.

The second action is identical to the first but uses the "notify.mobile_app_Second_Phone" service to clear or dismiss the notification on another mobile device with the "intercom" tag.

Mode: The mode is set to "single," which means that the automation will only run once for each trigger event. Subsequent trigger events with the "intercom_ignore" action will trigger this automation, but it will only clear the notification once.
</details>

```yaml
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

### Automation _3_
This automatization enables the Relays R1/R2 by publishing the MQTT payload and clears notifications with the "tag: intercom" on phones when the notification button is pressed on one of the phones.
Solid-state relays are controlled by publishing the MQTT payload. MQTT Command Topic is "homeassistant/sensor/espbell-max/command" Payload is "00" The first digit is controlling R1 second digit R2.

* "01" R1=OFF R2=ON
* "11" R1=ON R2=ON
* "10" R1=ON R2=OFF
* "00" R1=OFF R2=OFF

```yaml
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

