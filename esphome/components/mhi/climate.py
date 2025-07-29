import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate_ir
from esphome.const import CONF_ID

AUTO_LOAD = ['climate_ir']

mhi_ns = cg.esphome_ns.namespace('mhi')
MhiClimate = mhi_ns.class_('MhiClimate', climate_ir.ClimateIR)

#CONFIG_SCHEMA = climate_ir.CLIMATE_IR_SCHEMA.extend({
CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(MhiClimate)


async def to_code(config):
    await climate_ir.new_climate_ir(config)
