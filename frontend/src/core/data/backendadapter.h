#ifndef BACKENDADAPTER_H
#define BACKENDADAPTER_H

#include <QObject>

#include "measurementpoint.h"

class SensorFrame;

class BackendAdapter : public QObject
{
    Q_OBJECT

public:
    explicit BackendAdapter(QObject *parent = nullptr);

public slots:
    void onSensorFrameReceived(const SensorFrame& frame);

    // MOCK
    void generateMockData();
};

#endif // BACKENDADAPTER_H
