#include "mhi.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mhi {

static const char *const TAG = "mhi.climate";

// Power
static const uint8_t MHI_OFF = 0x08;
static const uint8_t MHI_ON = 0x00;

// Operating mode
static const uint8_t MHI_AUTO = 0x07;
static const uint8_t MHI_HEAT = 0x03;
static const uint8_t MHI_COOL = 0x06;
static const uint8_t MHI_DRY = 0x05;
static const uint8_t MHI_FAN = 0x04;

// Fan speed
static const uint8_t MHI_FAN_AUTO = 0x0F;
static const uint8_t MHI_FAN1 = 0x0E;
static const uint8_t MHI_FAN2 = 0x0D;
static const uint8_t MHI_FAN3 = 0x0C;
static const uint8_t MHI_FAN4 = 0x0B;
static const uint8_t MHI_HIPOWER = 0x04;

// Vertical swing
static const uint8_t MHI_VS_SWING = 0xE0;
static const uint8_t MHI_VS_UP = 0xC0;
static const uint8_t MHI_VS_MUP = 0xA0;
static const uint8_t MHI_VS_MIDDLE = 0x80;
static const uint8_t MHI_VS_MDOWN = 0x60;
static const uint8_t MHI_VS_DOWN = 0x40;
static const uint8_t MHI_VS_STOP = 0x20;

// Horizontal swing
static const uint8_t MHI_HS_SWING = 0x0F;
static const uint8_t MHI_HS_MIDDLE = 0x0C;
static const uint8_t MHI_HS_LEFT = 0x0E;
static const uint8_t MHI_HS_MLEFT = 0x0D;
static const uint8_t MHI_HS_MRIGHT = 0x0B;
static const uint8_t MHI_HS_RIGHT = 0x0A;
static const uint8_t MHI_HS_STOP = 0x07;
static const uint8_t MHI_HS_LEFTRIGHT = 0x08;
static const uint8_t MHI_HS_RIGHTLEFT = 0x09;

// Only available in Auto, Cool and Heat mode
static const uint8_t MHI_3DAUTO_ON = 0x00;
static const uint8_t MHI_3DAUTO_OFF = 0x12;

// NOT available in Fan or Dry mode
static const uint8_t MHI_SILENT_ON = 0x00;
static const uint8_t MHI_SILENT_OFF = 0x80;

// Night setback
static const uint8_t MHI_NIGHT_ON = 0x00;
static const uint8_t MHI_NIGHT_OFF = 0x40;

// Eco
static const uint8_t MHI_ECO_ON = 0x00;
static const uint8_t MHI_ECO_OFF = 0x10;

// Pulse parameters in usec
static const uint16_t MHI_BIT_MARK = 400;
static const uint16_t MHI_ONE_SPACE = 1200;
static const uint16_t MHI_ZERO_SPACE = 400;
static const uint16_t MHI_HEADER_MARK = 3200;
static const uint16_t MHI_HEADER_SPACE = 1600;
static const uint16_t MHI_MIN_GAP = 17500;

static const uint8_t MHI_FRAME_SIZE = 19;

bool MhiClimate::on_receive(remote_base::RemoteReceiveData data) {
  ESP_LOGD(TAG, "Received some bytes");

  uint8_t bytes[MHI_FRAME_SIZE] = {};

  if (!data.expect_item(MHI_HEADER_MARK, MHI_HEADER_SPACE))
    return false;

  for (uint8_t a_byte = 0; a_byte < MHI_FRAME_SIZE; a_byte++) {
    uint8_t byte = 0;
    for (int8_t a_bit = 0; a_bit < 8; a_bit++) {
      if (data.expect_item(MHI_BIT_MARK, MHI_ONE_SPACE))
        byte |= 1 << a_bit;
      else if (!data.expect_item(MHI_BIT_MARK, MHI_ZERO_SPACE))
        return false;
    }
    bytes[a_byte] = byte;
  }

  ESP_LOGD(TAG, "Received bytes: %s", format_hex_pretty(bytes, MHI_FRAME_SIZE).c_str());

  // Check the static bytes
  if (bytes[0] != 0x52 || bytes[1] != 0xAE || bytes[2] != 0xC3 || bytes[3] != 0x1A || bytes[4] != 0xE5) {
    return false;
  }

  ESP_LOGD(TAG, "Passed check 1");

  // Check the inversed bytes
  if (bytes[5] != (~bytes[6] & 0xFF) || bytes[7] != (~bytes[8] & 0xFF) || bytes[9] != (~bytes[10] & 0xFF) ||
      bytes[11] != (~bytes[12] & 0xFF) || bytes[13] != (~bytes[14] & 0xFF) || bytes[15] != (~bytes[16] & 0xFF) ||
      bytes[17] != (~bytes[18] & 0xFF)) {
    return false;
  }

  ESP_LOGD(TAG, "Passed check 2");

  uint8_t power_mode = bytes[5] & 0x08;
  uint8_t operation_mode = bytes[5] & 0x07;
  uint8_t temperature = (~bytes[7] & 0x0F) + 17;
  uint8_t fan_speed = bytes[9] & 0x0F;
  uint8_t swing_v = bytes[11] & 0xE0;  // ignore the bit for the 3D auto
  uint8_t swing_h = bytes[13] & 0x0F;

  ESP_LOGD(TAG,
           "Resulting numbers: power_mode=0x%02X operation_mode=0x%02X temperature=%d fan_speed=0x%02X "
           "swing_v=0x%02X swing_h=0x%02X",
           power_mode, operation_mode, temperature, fan_speed, swing_v, swing_h);

  if (power_mode == MHI_ON) {
    switch (operation_mode) {
      case MHI_COOL:
        this->mode = climate::CLIMATE_MODE_COOL;
        break;
      case MHI_HEAT:
        this->mode = climate::CLIMATE_MODE_HEAT;
        break;
      case MHI_FAN:
        this->mode = climate::CLIMATE_MODE_FAN_ONLY;
        break;
      case MHI_DRY:
        this->mode = climate::CLIMATE_MODE_DRY;
        break;
      case MHI_AUTO:
        // climate_ir has no AUTO mode; HEAT_COOL is the closest equivalent
        this->mode = climate::CLIMATE_MODE_HEAT_COOL;
        break;
      default:
        break;
    }
  } else {
    this->mode = climate::CLIMATE_MODE_OFF;
  }

  this->target_temperature = temperature;

  // Horizontal and vertical swing
  if (swing_v == MHI_VS_SWING && swing_h == MHI_HS_SWING) {
    this->swing_mode = climate::CLIMATE_SWING_BOTH;
  } else if (swing_v == MHI_VS_SWING) {
    this->swing_mode = climate::CLIMATE_SWING_VERTICAL;
  } else if (swing_h == MHI_HS_SWING) {
    this->swing_mode = climate::CLIMATE_SWING_HORIZONTAL;
  } else {
    this->swing_mode = climate::CLIMATE_SWING_OFF;
  }

  // Fan speed
  switch (fan_speed) {
    case MHI_FAN1:
      this->fan_mode = climate::CLIMATE_FAN_LOW;
      break;
    case MHI_FAN2:  // Only to support remote feedback
    case MHI_FAN3:
      this->fan_mode = climate::CLIMATE_FAN_MEDIUM;
      break;
    case MHI_FAN4:
      this->fan_mode = climate::CLIMATE_FAN_HIGH;
      break;
    case MHI_FAN_AUTO:
      this->fan_mode = climate::CLIMATE_FAN_AUTO;
      break;
    case MHI_HIPOWER:
      // Set via BOOST preset; feedback parsing is best-effort
      this->preset = climate::CLIMATE_PRESET_BOOST;
      break;
    default:
      this->fan_mode = climate::CLIMATE_FAN_AUTO;
      break;
  }

  this->publish_state();
  return true;
}

void MhiClimate::transmit_state() {
  uint8_t remote_state[MHI_FRAME_SIZE] = {0x52, 0xAE, 0xC3, 0x1A, 0xE5, 0x90, 0x00, 0xF0, 0x00, 0xE0,
                                          0x00, 0x0D, 0x00, 0x10, 0x00, 0x3F, 0x00, 0x7F, 0x00};

  // Initial values
  uint8_t operating_mode = MHI_AUTO;
  uint8_t power_mode = MHI_ON;
  const uint8_t clean_mode = 0x60;  // always off

  uint8_t temperature = 22;
  uint8_t fan_speed = MHI_FAN_AUTO;
  uint8_t swing_v = MHI_VS_STOP;
  uint8_t swing_h = MHI_HS_STOP;
  uint8_t auto_3d = MHI_3DAUTO_OFF;
  uint8_t eco_mode = MHI_ECO_OFF;
  uint8_t silent_mode = MHI_SILENT_OFF;
  uint8_t night_mode = MHI_NIGHT_OFF;

  // Power and operating mode
  switch (this->mode) {
    case climate::CLIMATE_MODE_HEAT_COOL:
      operating_mode = MHI_AUTO;
      swing_v = MHI_VS_MIDDLE;
      break;
    case climate::CLIMATE_MODE_COOL:
      operating_mode = MHI_COOL;
      swing_v = MHI_VS_UP;
      break;
    case climate::CLIMATE_MODE_HEAT:
      operating_mode = MHI_HEAT;
      swing_v = MHI_VS_DOWN;
      break;
    case climate::CLIMATE_MODE_FAN_ONLY:
      operating_mode = MHI_FAN;
      swing_v = MHI_VS_MIDDLE;
      break;
    case climate::CLIMATE_MODE_DRY:
      operating_mode = MHI_DRY;
      swing_v = MHI_VS_MIDDLE;
      break;
    case climate::CLIMATE_MODE_OFF:
      power_mode = MHI_OFF;
      break;
    default:
      break;
  }

  // Temperature
  if (this->target_temperature > 17 && this->target_temperature < 31)
    temperature = this->target_temperature;

  // Horizontal and vertical swing
  switch (this->swing_mode) {
    case climate::CLIMATE_SWING_BOTH:
      swing_v = MHI_VS_SWING;
      swing_h = MHI_HS_SWING;
      break;
    case climate::CLIMATE_SWING_HORIZONTAL:
      swing_h = MHI_HS_SWING;
      break;
    case climate::CLIMATE_SWING_VERTICAL:
      swing_v = MHI_VS_SWING;
      break;
    case climate::CLIMATE_SWING_OFF:
    default:
      break;
  }

  // Fan speed
  switch (this->fan_mode.value()) {
    case climate::CLIMATE_FAN_LOW:
      fan_speed = MHI_FAN1;
      break;
    case climate::CLIMATE_FAN_MEDIUM:
      fan_speed = MHI_FAN3;
      break;
    case climate::CLIMATE_FAN_HIGH:
      fan_speed = MHI_FAN4;
      break;
    case climate::CLIMATE_FAN_AUTO:
      fan_speed = MHI_FAN_AUTO;
      break;
    default:
      fan_speed = MHI_FAN_AUTO;
      break;
  }

  switch (this->preset.value()) {
    case climate::CLIMATE_PRESET_NONE:
      auto_3d = MHI_3DAUTO_OFF;
      eco_mode = MHI_ECO_OFF;
      night_mode = MHI_NIGHT_OFF;
      break;
    case climate::CLIMATE_PRESET_ECO:
      auto_3d = MHI_3DAUTO_OFF;
      eco_mode = MHI_ECO_ON;
      night_mode = MHI_NIGHT_OFF;
      fan_speed = MHI_FAN2;
      break;
    case climate::CLIMATE_PRESET_BOOST:
      auto_3d = MHI_3DAUTO_OFF;
      fan_speed = MHI_HIPOWER;
      night_mode = MHI_NIGHT_OFF;
      break;
    case climate::CLIMATE_PRESET_ACTIVITY:
      auto_3d = MHI_3DAUTO_ON;
      night_mode = MHI_NIGHT_OFF;
      break;
    case climate::CLIMATE_PRESET_SLEEP:
      auto_3d = MHI_3DAUTO_OFF;
      night_mode = MHI_NIGHT_ON;
      fan_speed = MHI_FAN1;
      break;
    default:
      break;
  }

  // Assign the bytes
  remote_state[5] |= power_mode | operating_mode | clean_mode;
  remote_state[7] |= (~((uint8_t) temperature - 17) & 0x0F);
  remote_state[9] |= fan_speed | eco_mode;
  remote_state[11] |= swing_v | auto_3d;
  // Horizontal air flow (low nibble); high nibble keeps default 0x10
  remote_state[13] |= swing_h;
  remote_state[15] |= silent_mode | night_mode;

  // There is no real checksum, but some bytes are inverted
  remote_state[6] = ~remote_state[5];
  remote_state[8] = ~remote_state[7];
  remote_state[10] = ~remote_state[9];
  remote_state[12] = ~remote_state[11];
  remote_state[14] = ~remote_state[13];
  remote_state[16] = ~remote_state[15];
  remote_state[18] = ~remote_state[17];

  ESP_LOGD(TAG, "Sent bytes: %s", format_hex_pretty(remote_state, MHI_FRAME_SIZE).c_str());

  auto transmit = this->transmitter_->transmit();
  auto *transmit_data = transmit.get_data();

  transmit_data->set_carrier_frequency(38000);

  // Header
  transmit_data->mark(MHI_HEADER_MARK);
  transmit_data->space(MHI_HEADER_SPACE);

  // Data
  for (uint8_t i : remote_state) {
    for (uint8_t j = 0; j < 8; j++) {
      transmit_data->mark(MHI_BIT_MARK);
      bool bit = i & (1 << j);
      transmit_data->space(bit ? MHI_ONE_SPACE : MHI_ZERO_SPACE);
    }
  }
  transmit_data->mark(MHI_BIT_MARK);
  transmit_data->space(0);

  transmit.perform();
}

}  // namespace mhi
}  // namespace esphome
