#pragma once
#include <QtGlobal>
#include <QString>

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

// Settings of model (for writing/reading operations)
struct ModelConfig
{
    bool motorEnabled;
    double targetRpm;
    double throttlePosition;
    double brakeTorque;
};

// States of model (for only reading operations)
struct ModelInfo {
    bool fanAd;
    bool fanBall;
    ModelConfig cfg;
};

// Data of sensors
struct SensorFrame
{
    qint64 runId;
    int stage;
    qint64 timestampMs;
    double rpm, torque;
    double dieselTemp, motorTemp, resistorTemp, dieselPressure;
    double throttle, brakeTorque;
};
//stages of experiment
enum DiagState{
    IDLE = 0,
    COLD_CRANKING, 
    START_AND_WARMUP, 
    HOT_NO_LOAD, 
    HOT_WITH_LOAD, 
    COMPLETED, 
    ABORTED
};