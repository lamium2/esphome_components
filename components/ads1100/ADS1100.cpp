#include "ADS1100.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ads1100 {

static const char *const TAG = "ads1100";
static const uint8_t ADS1100_REGISTER_CONVERSION = 0x00;
static const uint8_t ADS1100_REGISTER_CONFIG = 0x00;

void ADS1100Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ADS1100...");
  uint16_t value;
  if (!this->read_byte_16(ADS1100_REGISTER_CONVERSION, &value)) {
    this->mark_failed();
    return;
  }

  ESP_LOGCONFIG(TAG, "Configuring ADS1100...");

  uint8_t config = 0;
  // Clear single-shot bit
  //        0b0xxxxxxx
  config |= 0b00000000;
  // Setup Gain
  //        0bxxxxxx00
  config |= GAIN_1;

  if (this->continuous_mode_) {
    // Set continuous mode
    //        0bxxx0xxxx
    config |= CONT;
  } else {
    // Set singleshot mode
    //        0bxxx1xxxx
    config |= SINGLE;
  }
  // Set data rate
  //        0B00001100
  config |= SPS_15;

  if (!this->write_byte(ADS1100_REGISTER_CONFIG, config)) {
    this->mark_failed();
    return;
  }
  this->prev_config_ = config;
}

void ADS1100Component::dump_config() {
  ESP_LOGCONFIG(TAG, "Setting up ADS1100...");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with ADS1100 failed!");
  }
}

float ADS1100Component::request_measurement(gain_t gain, res_t resolution) {
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
    if (!this->write_byte(ADS1100_REGISTER_CONFIG, config)) {
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
      while (this->read_byte_16(ADS1100_REGISTER_CONFIG, &config) && (config >> 15) == 0) {
        if (millis() - start > 100) {
          ESP_LOGW(TAG, "Reading ADS1100 timed out");
          this->status_set_warning();
          return NAN;
        }
        yield();
      }
    } */
  }

  uint16_t raw_conversion;
  if (!this->read_byte_16(ADS1100_REGISTER_CONVERSION, &raw_conversion)) {
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