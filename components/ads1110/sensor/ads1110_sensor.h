#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/voltage_sampler/voltage_sampler.h"

#include "../ADS1110.h"

namespace esphome {
namespace ads1110 {

/// Internal holder class that is in instance of Sensor so that the hub can create individual sensors.
class ADS1110Sensor : public sensor::Sensor,
                      public PollingComponent,
                      public voltage_sampler::VoltageSampler,
                      public Parented<ADS1110Component> {
 public:
  void update() override;
  void set_gain(gain_t gain) { this->gain_ = gain; }
  void set_resolution(res_t resolution) { this->resolution_ = resolution; }
  float sample() override;

  void dump_config() override;

 protected:
  gain_t gain_;
  res_t resolution_;
};

}  // namespace ads1110
}  // namespace esphome
