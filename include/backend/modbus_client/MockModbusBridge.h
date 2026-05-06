#pragma once
#include "IModbusBridge.h"
#include <QTimer>

class ModbusConfig;
class ModelConfig;
class ModelControl;

class MockModbusBridge : public IModbusBridge
{
    Q_OBJECT
public:
    explicit MockModbusBridge(const ModbusConfig& cfg, QObject* parent = nullptr);
public slots:
    void startPolling() override;
    void stopPolling() override;
    void onReadSensors() override;
    void onReadInfo() override;
    void onWriteConfig(const ModelConfig& cmd) override;
    void onWriteControl(const ModelControl& control) override;
private slots:
    void requestSensors();
    void requestInfo();
private:
    ModbusConfig m_cfg;
    QTimer m_pollTimer;
};
