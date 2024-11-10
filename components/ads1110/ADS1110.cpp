#include "ADS1110.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ads1110 {

static const char *const TAG = "ads1110";
static const uint8_t ADS1110_REGISTER_CONVERSION = 0x00;
static const uint8_t ADS1110_REGISTER_CONFIG = 0x00;

void ADS1110Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ADS1110...");
  uint16_t value;
  if (!this->read_byte_16(ADS1110_REGISTER_CONVERSION, &value)) {
    this->mark_failed();
    return;
  }

  ESP_LOGCONFIG(TAG, "Configuring ADS1110...");

  uint8_t config = DEFAULT_CONFIG;
    if (!this->continuous_mode_) {
    // Set singleshot mode
    //        0bxxx1xxxx
    config |= SINGLE;
  }

  if (!this->write_byte(ADS1110_REGISTER_CONFIG, config)) {
    this->mark_failed();
    return;
  }
  this->prev_config_ = config;
}

void ADS1110Component::dump_config() {
  ESP_LOGCONFIG(TAG, "Setting up ADS1110...");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with ADS1110 failed!");
  }
}

float ADS1110Component::request_measurement(gain_t gain, res_t resolution) {
  uint8_t config = this->prev_config_;

  // Gain
  //        0bxxxxxxXX
  config |= (gain & GAIN_MASK);

  // Resolution
  //        0BxxxxXXxx
  config |= (resolution & SPS_MASK);


  if (!this->continuous_mode_) {
    // Start conversion
    config |= START_CONVERSION;
  }

  if (!this->continuous_mode_ || this->prev_config_ != config) {
    if (!this->write_byte(ADS1110_REGISTER_CONFIG, config)) {
      this->status_set_warning();
      return NAN;
    }
    this->prev_config_ = config;

    // about 67 ms with 15 samples per second
    delay(70);

    // in continuous mode, conversion will always be running, rely on the delay
    // to ensure conversion is taking place with the correct settings
    // can we use the rdy pin to trigger when a conversion is done?
    /* if (!this->continuous_mode_) {
      uint32_t start = millis();
      while (this->read_byte_16(ADS1110_REGISTER_CONFIG, &config) && (config >> 15) == 0) {
        if (millis() - start > 100) {
          ESP_LOGW(TAG, "Reading ADS1110 timed out");
          this->status_set_warning();
          return NAN;
        }
        yield();
      }
    } */
  }

  uint16_t raw_conversion;
  if (!this->read_byte_16(ADS1110_REGISTER_CONVERSION, &raw_conversion)) {
    this->status_set_warning();
    return NAN;
  }

  auto signed_conversion = static_cast<int16_t>(raw_conversion);

  float millivolts;
  float divider = 2048.0f;
  switch (gain) {
    case GAIN_1:
      millivolts = signed_conversion / divider;
      break;
    case GAIN_2:
      millivolts = (signed_conversion * 2) / divider;
      break;
    case GAIN_4:
      millivolts = (signed_conversion * 4) / divider;
      break;
    case GAIN_8:
      millivolts = (signed_conversion * 8) / divider;
      break;
    default:
      millivolts = NAN;
  }

  this->status_clear_warning();
  return millivolts / 1e3f;
}

}
}