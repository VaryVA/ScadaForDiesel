#include "backend/DataTypes.h"
#include "backend/modbus_client/MockModbusBridge.h"
#include <QObject>
#include <QDebug>
#include <QThread>

MockModbusBridge::MockModbusBridge(const ModbusConfig& cfg, QObject* parent) : IModbusBridge(cfg, parent)
{
    m_cfg = cfg;
    QObject::connect(&m_pollTimer, &QTimer::timeout, this, &MockModbusBridge::onReadSensors);
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
    qInfo() << "[MockModbusBridge] Requesting data";
    SensorFrame frame = { 1, 1, 1, 1, 1, 1, 1 };
    emit sensorsDataReady(frame);
}

void MockModbusBridge::onReadInfo()
{
    qInfo() << "[MockModbusBridge] Requesting model info";
}

void MockModbusBridge::onWriteConfig(const ModelConfig& cmd)
{
    qInfo() << "[MockModbusBridge] Writing a new configuration to the model";
}

void MockModbusBridge::onWriteDecision(const Decision& decision)
{
    qInfo() << "[MockModbusBridge] Sending a decision to the model";
}
