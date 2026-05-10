#include "backendadapter.h"

#include <QRandomGenerator>

#include "datamanager.h"

BackendAdapter::BackendAdapter(QObject *parent)
    : QObject(parent)
{
}

void BackendAdapter::onSensorFrameReceived(const SensorFrame &frame)
{
    Q_UNUSED(frame);

    // TODO:
    // Когда backend даст структуру SensorFrame,
    // здесь будет преобразование в MeasurementPoint

    MeasurementPoint point;

    point.timestamp = QDateTime::currentDateTime();

    point.oilTemperature = 70;
    point.oilPressure = 1.5;
    point.rpm = 3000;
    point.torque = 250;

    DataManager::instance().appendMeasurement(point);
}
//mock
void BackendAdapter::generateMockData()
{
    MeasurementPoint point;

    point.timestamp = QDateTime::currentDateTime();

    point.oilTemperature =
        70 + QRandomGenerator::global()->bounded(20);

    point.oilPressure =
        1.0 + QRandomGenerator::global()->bounded(20) / 10.0;

    point.rpm =
        2500 + QRandomGenerator::global()->bounded(1000);

    point.torque =
        200 + QRandomGenerator::global()->bounded(100);

    DataManager::instance().appendMeasurement(point);
}
