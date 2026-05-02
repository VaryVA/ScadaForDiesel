#include "MockModbusConfigBuilder.h"

std::optional<ModbusConfig> MockModbusConfigBuilder::get() const
{
    return ModbusConfig{
        "localhost",
        502,
        1000,
        1000,
        3,
        1
    };
}
