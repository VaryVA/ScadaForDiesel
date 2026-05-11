#ifndef MEASUREMENTPOINT_H
#define MEASUREMENTPOINT_H

#include <QDateTime>

struct MeasurementPoint
{
    QDateTime timestamp;

    double oilTemperature = 0.0;
    double oilPressure = 0.0;
    double rpm = 0.0;
    double torque = 0.0;
};

#endif // MEASUREMENTPOINT_H
