# esphome-climate-mhi
### This is to TEST CUSTOM PRESET
Custom component to support Mitsubishi Heavy Industries as climate IR in ESPhome.

This code is based on remote type RLA502A700K
The remote was delivered with indoor unit model SRKxxZSW-W.

Also possibel to use remote reciver in ESPHome to read out IR commands sent by IR Remote.

To use this, use following config in ESPHome

```
external_components:
  source:
    type: git
    url: https://github.com/karllinder/esphome-climate-mhi
    ref: Add_ECO_Preset
  refresh: 1min
```
Then, add the climate config:

```
remote_receiver:
  id: rcvr
  pin:
    number: GPIOxx
    inverted: true
    mode:
      input: true
      pullup: true

remote_transmitter:
  pin: GPIOxx
  carrier_duty_percent: 50%
  
climate:
   - platform: mhi
     name: "MHI"
     receiver_id: rcvr

```
