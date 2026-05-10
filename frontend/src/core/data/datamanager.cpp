#include "datamanager.h"

DataManager::DataManager(QObject *parent)
    : QObject(parent)
{
}

DataManager& DataManager::instance()
{
    static DataManager instance;
    return instance;
}

void DataManager::appendMeasurement(const MeasurementPoint &point)
{
    m_measurements.append(point);

    if(m_measurements.size() > MAX_POINTS)
    {
        m_measurements.removeFirst();
    }

    emit newMeasurement(point);
}

QVector<MeasurementPoint> DataManager::measurements() const
{
    return m_measurements;
}
