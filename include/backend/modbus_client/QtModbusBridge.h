#pragma once
#include "IModbusBridge.h"
#include <QTimer>
#include <QModbusDataUnit>
#include <QModbusTcpClient>

class ModbusConfig;
class ModelConfig;
class Decision;

class QtModbusBridge : public IModbusBridge
{
    Q_OBJECT
public:
    explicit QtModbusBridge(const ModbusConfig& cfg, QObject* parent = nullptr);
    ~QtModbusBridge() override;
public slots:
    void startPolling() override;
    void stopPolling() override;
    void onReadSensors() override;
    void onReadInfo() override;
    void onWriteConfig(const ModelConfig& cmd) override;
    void onWriteDecision(const Decision& decision) override;
private slots:
    void parseSensorsResponse(QModbusReply* reply);
    void requestSensors();
    void parseInfoResponse(QModbusReply* reply);
    void requestInfo();
private:
    ModbusConfig m_cfg;
    QModbusDataUnit m_dataUnit;
    QModbusTcpClient m_client;
    QTimer m_pollTimer;
    QModbusDevice::State m_prevState = QModbusDevice::UnconnectedState;
};
