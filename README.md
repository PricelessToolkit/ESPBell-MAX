<img src="https://raw.githubusercontent.com/PricelessToolkit/ESPBell-MAX/main/img/banner.jpg"/>

🤗 If you value the effort I've put into crafting this open-source project and would like to show your support, consider subscribing to my [YouTube channel](https://www.youtube.com/@PricelessToolkit/videos). Your subscription goes a long way in backing my work.
# ESPBell-MAX
 Intercom / Doorbell Module

## Home Assistant Configuration
We need to create 3 automations

### Automation 1 "Intecom DoorBell Notification"
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
        channel: interphone
        tag: interphone
        image: /media/local/notify/doorbell.jpg
        actions:
          - action: interphone_ignore
            title: Ignore ✖
          - action: interphone_open
            title: Open The Door 🔓
      title: DoorBell 🔔
      message: Quelqu'un à la porte
  - service: notify.mobile_app_Second_Phone
    data:
      message: Someone at the door
      data:
        persistent: true
        importance: high
        channel: interphone
        tag: interphone
        image: /media/local/notify/doorbell.jpg
        actions:
          - action: interphone_ignore
            title: Ignore ✖
          - action: interphone_open
            title: Open The Door 🔓
      title: DoorBell 🔔
      message: Quelqu'un à la porte
mode: single

```

### Automation 2 "Intercom DoorBell ignore notification 'Dismiss'"
```
alias: 🔔 Intercom DoorBell ignore notification Dismiss
description: ""
trigger:
  - platform: event
    event_data:
      action: interphone_ignore
    event_type: mobile_app_notification_action
condition: []
action:
  - service: notify.mobile_app_doogee_v20pro
    data:
      message: clear_notification
      data:
        tag: interphone
  - service: notify.mobile_app_Second_Phone
    data:
      message: clear_notification
      data:
        tag: interphone
mode: single
```

### Automation 3 "Intercom open the door and Dismiss notification"
```
alias: 🔔 Intercom open the door and Dismiss notification
description: ""
trigger:
  - platform: event
    event_data:
      action: interphone_open
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
        tag: interphone
  - service: notify.mobile_app_Second_Phone
    data:
      message: clear_notification
      data:
        tag: interphone
mode: single

```
