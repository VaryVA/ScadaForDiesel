#pragma once
#include "IModbusConfigBuilder.h"

class MockModbusConfigBuilder : public IModbusConfigBuilder
{
public:
    std::optional<ModbusConfig> get() const override;
};
