#include "ModbusConfigBuilder.h"

std::optional<ModbusConfig> ModbusConfigBuilder::get() const
{
    return ModbusConfig{
        "localhost",
        502,
        1000,
        1000,
        3,
        1,
    };
}
