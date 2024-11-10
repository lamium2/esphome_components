#include "ads1100_sensor.h"

#include "esphome/core/log.h"

namespace esphome {
namespace ads1100 {

static const char *const TAG = "ads1100.sensor";

float ADS1100Sensor::sample() {
  return this->parent_->request_measurement(this->gain_, this->resolution_);
}

void ADS1100Sensor::update() {
  float v = this->sample();
  if (!std::isnan(v)) {
    ESP_LOGD(TAG, "'%s': Got Voltage=%fV", this->get_name().c_str(), v);
    this->publish_state(v);
  }
}

void ADS1100Sensor::dump_config() {
  LOG_SENSOR("  ", "ADS1100 Sensor", this);
  ESP_LOGCONFIG(TAG, "    Gain: %u", this->gain_);
  ESP_LOGCONFIG(TAG, "    Resolution: %u", this->resolution_);
}

}  // namespace ads1100
}  // namespace esphome
