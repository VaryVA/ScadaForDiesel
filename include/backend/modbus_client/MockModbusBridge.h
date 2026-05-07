#pragma once
#include "IModbusBridge.h"
#include "backend/DataTypes.h"
#include <QTimer>

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
    void onWriteDecision(const Decision& decision) override;
private:
    ModbusConfig m_cfg;
    QTimer m_pollTimer;
};
