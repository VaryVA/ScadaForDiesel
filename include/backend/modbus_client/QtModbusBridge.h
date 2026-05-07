#pragma once
#include "IModbusBridge.h"
#include "backend/DataTypes.h"
#include <QTimer>
#include <QModbusDataUnit>
#include <QModbusTcpClient>

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
    void onStateChange(QModbusDevice::State state);
    void onErrorOccured();
private:
    QModbusReply* sendReadRequest(const QModbusDataUnit& dataUnit);
    QModbusReply* sendWriteRequest(const QModbusDataUnit& dataUnit);
    void parseSensorsResponse(const QVector<quint16>& values);
    void parseInfoResponse(const QVector<quint16>& values);
    std::optional<QVector<quint16>> extractValues(QModbusReply* reply, qsizetype expectedSize);
private:
    ModbusConfig m_cfg;
    QModbusTcpClient m_client;
    QTimer m_pollTimer;
    QModbusDevice::State m_prevState = QModbusDevice::UnconnectedState;
};
