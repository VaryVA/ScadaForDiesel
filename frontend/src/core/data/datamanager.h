#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QVector>

#include "measurementpoint.h"

class DataManager : public QObject
{
    Q_OBJECT

public:
    static DataManager& instance();

    void appendMeasurement(const MeasurementPoint& point);

    QVector<MeasurementPoint> measurements() const;

signals:
    void newMeasurement(const MeasurementPoint& point);

private:
    explicit DataManager(QObject *parent = nullptr);

    QVector<MeasurementPoint> m_measurements;

    static constexpr int MAX_POINTS = 10000;
};

#endif // DATAMANAGER_H
