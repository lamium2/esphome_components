import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, voltage_sampler
from esphome.const import (
    CONF_GAIN,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_VOLT,
    CONF_ID,
)
from . import ads1110_ns, ADS1110Component

DEPENDENCIES = ["ads1110"]

ADS1110Gain = ads1110_ns.enum("ADS1110Gain")
GAIN = {
    "1": ADS1110Gain.ADS1110_GAIN_1,
    "2": ADS1110Gain.ADS1110_GAIN_2,
    "4": ADS1110Gain.ADS1110_GAIN_4,
    "8": ADS1110Gain.ADS1110_GAIN_8,
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
            cv.Required(CONF_GAIN): validate_gain,
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

    cg.add(paren.register_sensor(var))
