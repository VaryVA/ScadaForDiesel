#include "DataTypes.h"
#include "MockModbusBridge.h"
#include <QObject>
#include <QDebug>
#include <QThread>

MockModbusBridge::MockModbusBridge(const ModbusConfig& cfg, QObject* parent) : IModbusBridge(cfg, parent)
{
    m_cfg = cfg;
    QObject::connect(&m_pollTimer, &QTimer::timeout, this, &MockModbusBridge::requestSensors);
    qInfo() << "[MockModbusBridge] Initialization is successfull";
    QThread::msleep(1000);
    qInfo() << "[MockModbusBridge] Connected to the Modbus Server";
}

void MockModbusBridge::startPolling()
{
    qInfo() << "[MockModbusBridge] Starting to poll server";
    m_pollTimer.start(m_cfg.pollFrequencyMs);
}

void MockModbusBridge::stopPolling()
{
    qInfo() << "[MockModbusBridge] Stopping to poll server";
    m_pollTimer.stop();
}

void MockModbusBridge::onReadSensors()
{
    qInfo() << "[MockModbusBridge] Core is directly requesting to read sensor data";
    requestSensors();
}

void MockModbusBridge::onReadInfo()
{
    qInfo() << "[MockModbusBridge] Core is directly requesting to read the model info";
    requestInfo();
}

void MockModbusBridge::onWriteConfig(const ModelConfig& cmd)
{
    qInfo() << "[MockModbusBridge] Writing a new configuration to the model";
}

void MockModbusBridge::requestSensors()
{
    qInfo() << "[MockModbusBridge] Requesting data";
    SensorFrame frame = { 1, 1, 1, 1, 1, 1, 1, 1 };
    emit sensorsDataReady(frame);
}

void MockModbusBridge::requestInfo()
{
    qInfo() << "[MockModbusBridge] Requesting model info";
}
