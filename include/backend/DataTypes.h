#pragma once
#include <QtGlobal>

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