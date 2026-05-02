#pragma once
#include "IModbusConfigBuilder.h"

class ModbusConfigBuilder : public IModbusConfigBuilder
{
public:
    std::optional<ModbusConfig> get() const override;
private:
    bool validate();
};
