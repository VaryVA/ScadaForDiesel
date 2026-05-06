#pragma once
#include "IModbusBridge.h"
#include <QTimer>

class ModbusConfig;
class ModelConfig;

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
private slots:
    void requestSensors();
    void requestInfo();
private:
    ModbusConfig m_cfg;
    QTimer m_pollTimer;
};
