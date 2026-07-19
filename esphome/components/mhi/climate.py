import esphome.codegen as cg

from esphome.components import climate_ir

AUTO_LOAD = ["climate_ir"]
CODEOWNERS = ["@coldreckon"]

mhi_ns = cg.esphome_ns.namespace("mhi")
MhiClimate = mhi_ns.class_("MhiClimate", climate_ir.ClimateIR)

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(MhiClimate)


async def to_code(config):
    await climate_ir.new_climate_ir(config)
