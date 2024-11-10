#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"

#include <vector>

namespace esphome {
namespace ads1110 {

    const uint8_t DEFAULT_CONFIG   =  12;      // B00001100 (16-BIT, 15 SPS, GAIN x1, CONTINUOUS)
    const uint8_t DEFAULT_DATA     =   0;      // default value of Raw Data registers
    const uint8_t START_CONVERSION = 128;      // B10000000 (employed in 'Single-Shot' Conversion Mode)
    const uint8_t COM_SUCCESS      =   0;      // I2C Communication Success (No Error)
    const uint8_t MIN_CON_TIME     =   5;      // minimum ADC Comversion time (in mS)
    const uint8_t NUM_BYTES        =   3;      // fixed number of bytes requested from the device
    const int  MAX_NUM_ATTEMPTS =   3;      // number of attempts to get new data from device

    enum gain_t {
        GAIN_MASK = 0x03,      // 3 - B00000011
        GAIN_1    = 0x00,      // 0 - B00000000 (Default)
        GAIN_2    = 0x01,      // 1 - B00000001
        GAIN_4    = 0x02,      // 2 - B00000010
        GAIN_8    = 0x03       // 3 - B00000011
    };

    enum sample_rate_t {
        SPS_MASK = 0x0C,       // 12 - B00001100
        SPS_15   = 0x0C,       // 12 - B00001100 (Default)
        SPS_30   = 0x08,       //  8 - B00001000
        SPS_60   = 0x04,       //  4 - B00000100
        SPS_240  = 0x00        //  0 - B00000000
    };

    enum con_mode_t {
        CONT          = 0x00,  // B00000000 (Default)
        SINGLE        = 0x10   // B00010000
    };

    enum min_code_t {
        MIN_CODE_240 = 0x01,   //  1 - Minimal Data Value for 240_SPS / -2048  (12-BIT)
        MIN_CODE_60  = 0x04,   //  4 - Minimal Data Value for 60_SPS  / -2048  (14-BIT)
        MIN_CODE_30  = 0x08,   //  8 - Minimal Data Value for 30_SPS  / -2048  (15-BIT)
        MIN_CODE_15  = 0x10    // 16 - Minimal Data Value for 15_SPS  / -2048  (16-BIT) (Default)
    };

    enum res_t {
        RES_12   = 0x00        // SPS  0 - B00000000,                // 12-BIT Resolution
        RES_14   = 0x04,       // SPS  4 - B00000100,                // 14-BIT Resolution
        RES_15   = 0x08,       // SPS  8 - B00001000,                // 15-BIT Resolution
        RES_16   = 0x0C,       // SPS 12 - B00001100 (Default)       // 16-BIT Resolution (Default)
    };

    enum vref_t {
        INT_REF =    0,        // Inernal Reference:  Pin Vin- is connected to GND (Default)
        EXT_REF = 2048         // External Reference: Pin Vin- is connected to 2.048V source
    };

class ADS1110Component : public Component, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  /// HARDWARE_LATE setup priority
  float get_setup_priority() const override { return setup_priority::DATA; }
  void set_continuous_mode(bool continuous_mode) { continuous_mode_ = continuous_mode; }

  /// Helper method to request a measurement from a sensor.
  float request_measurement(gain_t gain, res_t resolution);

 protected:
  uint16_t prev_config_{0};
  bool continuous_mode_;
};
}
}

