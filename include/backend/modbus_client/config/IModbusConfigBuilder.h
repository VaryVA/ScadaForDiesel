#pragma once
#include <QString>
#include <QtGlobal>

// Control registers mapping, read/write, 1-bit
struct CoilsRegistersScheme
{
    bool motorEnabled;
};

// Device status registers mapping, only read, 1-bit
struct DiscreteRegistersScheme
{
    bool fanAd;
    bool fanBall;
};

// Settings and variables registers mapping, read/write, 16-bit
struct HoldingRegistersScheme
{
    double targetRpm;
    double throttlePosition;
    double brakeTorque;
};

// Sensor data registers mapping, only read, 16-bit
struct InputRegistersScheme
{
    double rpm, torque;
    double dieselTemp, motorTemp, resistorTemp, dieselPressure;
    double throttle, brakeTorque;
};

struct ModbusConfig
{
    QString host;
    quint16 port = 1502;
    int pollFrequencyMs = 1000;
    int timeoutMs = 1000;
    int retries = 3;
    int unitId = 1;  // we assume one modbus device, so unitId will be ignored
    // Start address is 0 for each register type
    CoilsRegistersScheme coils;
    DiscreteRegistersScheme discrete;
    InputRegistersScheme input;
    HoldingRegistersScheme holding;
};

class IModbusConfigBuilder
{
public:
    virtual std::optional<ModbusConfig> get() const = 0;
};
