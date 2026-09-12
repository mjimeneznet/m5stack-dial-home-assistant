import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.components.sensor as sensor
import esphome.components.text_sensor as text_sensor
from esphome.const import CONF_ENTITY_ID, CONF_ID, CONF_NAME

CODEOWNERS = []
DEPENDENCIES = ["api", "sensor", "text_sensor"]

CONF_TARGET_TEMP_SENSOR = "target_temp_sensor"
CONF_CURRENT_TEMP_SENSOR = "current_temp_sensor"
CONF_HUMIDITY_SENSOR = "humidity_sensor"
CONF_HVAC_MODE_SENSOR = "hvac_mode_sensor"
CONF_HVAC_MODES_SENSOR = "hvac_modes_sensor"
CONF_FAN_MODE_SENSOR = "fan_mode_sensor"
CONF_FAN_MODES_SENSOR = "fan_modes_sensor"

dial_climates_ns = cg.esphome_ns.namespace("dial_climates")
DialClimates = dial_climates_ns.class_("DialClimates", cg.Component)

CLIMATE_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ENTITY_ID): cv.string,
        cv.Required(CONF_NAME): cv.string,
        cv.Optional(CONF_TARGET_TEMP_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_CURRENT_TEMP_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_HUMIDITY_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_HVAC_MODE_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_HVAC_MODES_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_FAN_MODE_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_FAN_MODES_SENSOR): cv.use_id(text_sensor.TextSensor),
    }
)


def _config_schema(value):
    climates = cv.All(cv.ensure_list(CLIMATE_SCHEMA), cv.Length(min=0))(value)
    return {
        CONF_ID: cv.declare_id(DialClimates)("dial_climates_id"),
        "climates": climates,
    }


CONFIG_SCHEMA = _config_schema


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    for climate in config["climates"]:
        entity = climate[CONF_ENTITY_ID]
        name = climate[CONF_NAME]
        target_temp = None
        current_temp = None
        humidity = None
        hvac_mode = None
        hvac_modes = None
        fan_mode = None
        fan_modes = None
        if CONF_TARGET_TEMP_SENSOR in climate:
            target_temp = await cg.get_variable(climate[CONF_TARGET_TEMP_SENSOR])
        if CONF_CURRENT_TEMP_SENSOR in climate:
            current_temp = await cg.get_variable(climate[CONF_CURRENT_TEMP_SENSOR])
        if CONF_HUMIDITY_SENSOR in climate:
            humidity = await cg.get_variable(climate[CONF_HUMIDITY_SENSOR])
        if CONF_HVAC_MODE_SENSOR in climate:
            hvac_mode = await cg.get_variable(climate[CONF_HVAC_MODE_SENSOR])
        if CONF_HVAC_MODES_SENSOR in climate:
            hvac_modes = await cg.get_variable(climate[CONF_HVAC_MODES_SENSOR])
        if CONF_FAN_MODE_SENSOR in climate:
            fan_mode = await cg.get_variable(climate[CONF_FAN_MODE_SENSOR])
        if CONF_FAN_MODES_SENSOR in climate:
            fan_modes = await cg.get_variable(climate[CONF_FAN_MODES_SENSOR])
        cg.add(
            var.add_climate(
                entity,
                name,
                target_temp,
                current_temp,
                humidity,
                hvac_mode,
                hvac_modes,
                fan_mode,
                fan_modes,
            )
        )
