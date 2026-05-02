#ifndef STRUCT_H
#define STRUCT_H

struct SensorFrame
{
    long runId;
    int stage;
    long timestampMs;
    double rpm, torque;
    double diselTemp, motorTemp, resistorTemp, dieselPressure;
    double throttle, brakeTorque;
};

enum EngineState
{
    Stop = 0,
    Run
};

struct CommandToBackend
{
    EngineState state;
    double rpm;
};


#endif // STRUCT_H
