# esphome-climate-mhi
### This is a clone of https://github.com/Dennis-Q/esphome-climate-mhi
The original repository is no longer updated, see https://github.com/Dennis-Q/esphome-climate-mhi/pull/2#issuecomment-1722956833

Custom component to support Mitsubishi Heavy Industries air conditioners as climate IR in ESPHome.

This code is based on remote type RLA502A700K.
The remote was delivered with indoor unit model SRKxxZSW-W.

Also possible to use remote receiver in ESPHome to read out IR commands sent by IR Remote.

To use this, use following config in ESPHome

```yaml
external_components:
  source:
    type: git
    url: https://github.com/karllinder/esphome-climate-mhi
```

## Branches

| Branch | Intended use | Tested against |
|---|---|---|
| `main` | Stable, day-to-day use | ESPHome 2026.4.0 |
| `beta` | Opt-in preview of the v2.0 code-quality refactor | ESPHome 2026.4.0 (CI: ESP8266 + ESP32 Arduino) |

### Trying the beta

The `beta` branch carries a round of ESPHome-style cleanups that are **behaviour-neutral** but represent a large diff:

- C++ reformatted to upstream ESPHome style (2-space indent, `snake_case` locals, `format_hex_pretty` for hex logging, dead code removed).
- `.clang-format` and `pyproject.toml` (ruff) for consistent local formatting/linting.
- GitHub Actions CI that compiles both `tests/test.esp8266-ard.yaml` and `tests/test.esp32-ard.yaml` on every PR.
- Python (`climate.py`) tidied: unused imports removed, `CODEOWNERS` added, double-quoted strings.

Same IR protocol, same supported modes/presets, same pinout — no behaviour change is intended. Once the beta has soaked for a while it will be merged into `main` as **v2.0**.

To opt in:

```yaml
external_components:
  source:
    type: git
    url: https://github.com/karllinder/esphome-climate-mhi
    ref: beta
```

Known open item: issue [#4](https://github.com/karllinder/esphome-climate-mhi/issues/4) tracks hardware validation of a small horizontal-swing byte change. Please comment there if you test.

Then, add the climate config:

```yaml
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

## Features

### Supported Modes
- **Cool** - Cooling mode
- **Heat** - Heating mode  
- **Dry** - Dehumidification mode
- **Fan** - Fan only mode
- **Auto** - Automatic mode (displayed as Heat/Cool in Home Assistant)

### Temperature Control
- Temperature range: 18°C - 30°C
- 1°C increments

### Fan Speed
- Auto
- Low
- Medium
- High

### Swing Modes
- Off
- Vertical swing
- Horizontal swing
- Both (vertical and horizontal)

### Presets
- **None** - Normal operation
- **Eco** - Energy saving mode (sets fan to speed 2)
- **Boost** - High power mode (maximum fan speed)
- **Activity** - 3D auto mode enabled
- **Sleep** - Night mode for quiet operation (sets fan to low speed)

### Night Mode (New Feature)
The night mode feature is now available through the **Sleep** preset. When activated:
- Enables the AC unit's built-in night setback mode, set the temperature to 10°C
- Automatically sets fan speed to low for quiet operation
- Disables 3D auto mode
- Perfect for nighttime use to ensure comfortable and quiet sleep

To activate night mode, simply select the "Sleep" preset in Home Assistant's climate control interface.

## Recent Changes
- Fixed `ClimateIR` constructor signature for ESPHome 2026.4.0 (`std::set<...>` → `FiniteSetMask`)
- Fixed deprecated ESPHome schema warnings for compatibility with ESPHome 2025.11.0
- Added night mode support via the Sleep preset
- Updated to use async/await syntax for ESPHome compatibility
- Opened a `beta` branch staging the v2.0 upstream-style refactor (see [Branches](#branches))
