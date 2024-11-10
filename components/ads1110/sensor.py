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
from . import ads1110_ns, ADS1110Component

DEPENDENCIES = ["ads1110"]

gain_t = ads1110_ns.enum("gain_t")
GAIN = {
    "1": gain_t.GAIN_1,
    "2": gain_t.GAIN_2,
    "4": gain_t.GAIN_4,
    "8": gain_t.GAIN_8,
}

min_code_t = ads1110_ns.enum("min_code_t")
RESOLUTION = {
    "16_BITS": min_code_t.RES_16,
    "15_BITS": min_code_t.RES_15,
    "14_BITS": min_code_t.RES_14,
    "12_BITS": min_code_t.RES_12,
}

def validate_gain(value):
    if isinstance(value, float):
        value = f"{value:0.03f}"
    elif not isinstance(value, str):
        raise cv.Invalid(f'invalid gain "{value}"')

    return cv.enum(GAIN)(value)


ADS1110Sensor = ads1110_ns.class_(
    "ADS1110Sensor", sensor.Sensor, cg.PollingComponent, voltage_sampler.VoltageSampler
)

CONF_ADS1110_ID = "ads1110_id"
CONFIG_SCHEMA = (
    sensor.sensor_schema(
        unit_of_measurement=UNIT_VOLT,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_VOLTAGE,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(
        {
            cv.GenerateID(): cv.declare_id(ADS1110Sensor),
            cv.GenerateID(CONF_ADS1110_ID): cv.use_id(ADS1110Component),
            cv.Required(CONF_GAIN): cv.enum(GAIN),
            cv.Optional(CONF_RESOLUTION, default="16_BITS"): cv.enum(
                RESOLUTION, upper=True, space="_"
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
)


async def to_code(config):
    paren = await cg.get_variable(config[CONF_ADS1110_ID])
    var = cg.new_Pvariable(config[CONF_ID], paren)
    await sensor.register_sensor(var, config)
    await cg.register_component(var, config)

    cg.add(var.set_gain(config[CONF_GAIN]))
    cg.add(var.set_resolution(config[CONF_RESOLUTION]))

    cg.add(paren.register_sensor(var))
