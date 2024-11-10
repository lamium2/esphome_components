import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, voltage_sampler
from esphome.const import (
    CONF_GAIN,
    CONF_RESOLUTION,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_VOLT,
    CONF_ID,
)
from .. import ads1100_ns, ADS1100Component, CONF_ADS1100_ID

AUTO_LOAD = ["voltage_sampler"]
DEPENDENCIES = ["ads1100"]

gain_t = ads1100_ns.enum("gain_t")
GAIN = {
    "1": gain_t.GAIN_1,
    "2": gain_t.GAIN_2,
    "4": gain_t.GAIN_4,
    "8": gain_t.GAIN_8,
}

res_t = ads1100_ns.enum("res_t")
RESOLUTION = {
    "16_BITS": res_t.RES_16,
    "15_BITS": res_t.RES_15,
    "14_BITS": res_t.RES_14,
    "12_BITS": res_t.RES_12,
}

ADS1100Sensor = ads1100_ns.class_(
    "ADS1100Sensor",
    sensor.Sensor,
    cg.PollingComponent,
    voltage_sampler.VoltageSampler,
    cg.Parented.template(ADS1100Component),
)

TYPE_ADC = "adc"

CONFIG_SCHEMA = cv.typed_schema(
    {
        TYPE_ADC: sensor.sensor_schema(
            ADS1100Sensor,
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=3,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        )
        .extend(
            {
                cv.GenerateID(CONF_ADS1100_ID): cv.use_id(ADS1100Component),
                cv.Required(CONF_GAIN): cv.enum(GAIN, string=True),
                cv.Optional(CONF_RESOLUTION, default="16_BITS"): cv.enum(
                    RESOLUTION, upper=True, space="_"
                ),
            }
        )
        .extend(cv.polling_component_schema("60s"))
    },
    default_type=TYPE_ADC,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await sensor.register_sensor(var, config)
    await cg.register_component(var, config)
    await cg.register_parented(var, config[CONF_ADS1100_ID])

    cg.add(var.set_gain(config[CONF_GAIN]))
    cg.add(var.set_resolution(config[CONF_RESOLUTION]))
