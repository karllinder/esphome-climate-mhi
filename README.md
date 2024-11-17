# esphome-climate-mhi
### This is a clone of https://github.com/Dennis-Q/esphome-climate-mhi
The orginal reposity is no longer updated, see https://github.com/Dennis-Q/esphome-climate-mhi/pull/2#issuecomment-1722956833

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
