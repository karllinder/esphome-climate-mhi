# ESPHome Climate MHI Component

## Project Overview
This is a custom ESPHome component that provides IR (infrared) climate control support for Mitsubishi Heavy Industries (MHI) air conditioners. It's a fork of the original repository at https://github.com/Dennis-Q/esphome-climate-mhi which is no longer maintained.

## GitHub Repository
- **Repository URL**: https://github.com/karllinder/esphome-climate-mhi
- **Clone**: `gh repo clone https://github.com/karllinder/esphome-climate-mhi`

## Development Workflow
### Branching Strategy
For major changes, please follow this workflow:
1. Create a new branch for your feature/fix: `git checkout -b feature/your-feature-name`
2. Make your changes and commit them
3. Push the branch to GitHub: `git push origin feature/your-feature-name`
4. Create a Pull Request (PR) for review
5. After approval, merge the PR into main

### Quick Fixes
Minor fixes (typos, small bug fixes) can be committed directly to main.

## Purpose
The component enables control of MHI air conditioners through ESPHome using infrared signals, specifically designed for remote type RLA502A700K (delivered with indoor unit model SRKxxZSW-W).

## Key Features
- Full climate control integration with ESPHome
- IR transmitter support for sending commands
- IR receiver support for reading commands from the original remote
- Temperature control (18°C - 30°C)
- Multiple operating modes: Auto, Heat, Cool, Dry, Fan
- Fan speed control: Auto, Low, Medium, High
- Swing modes: Vertical, Horizontal, Both, Off
- Presets: None, Eco, Boost, Activity
- Special features: 3D Auto, Silent mode

## Repository Structure
```
esphome-climate-mhi/
├── README.md                    # User documentation
├── esphome/
│   └── components/
│       └── mhi/                # MHI component implementation
│           ├── __init__.py     # Python initialization (empty)
│           ├── climate.py      # ESPHome component registration
│           ├── mhi.h           # C++ header with class definition
│           └── mhi.cpp         # C++ implementation
```

## Technical Details

### Component Architecture
- **climate.py**: Registers the component with ESPHome, extends `CLIMATE_IR_WITH_RECEIVER_SCHEMA`
- **mhi.h**: Defines the `MhiClimate` class inheriting from `climate_ir::ClimateIR`
- **mhi.cpp**: Implements IR protocol encoding/decoding for MHI air conditioners

### IR Protocol
- Uses specific timing parameters for MHI protocol
- Header: 3200µs mark, 1600µs space
- Bit encoding: 400µs mark with 1200µs space (1) or 400µs space (0)
- Sends 19 bytes of data per command

### Recent Changes
- Night mode feature was recently removed (see commit history)
- Custom preset handling was commented out
- Hi-Power mode mapped to BOOST preset

## Usage Example
```yaml
external_components:
  source:
    type: git
    url: https://github.com/karllinder/esphome-climate-mhi

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

## Development Notes
- The component uses ESPHome's climate_ir base classes for standard functionality
- IR protocol implementation is specific to MHI RLA502A700K remote
- Logging available under TAG "mhi.climate" for debugging
- Supports both transmitting commands and receiving/decoding remote signals