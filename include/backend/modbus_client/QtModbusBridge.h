#pragma once
#include "IModbusBridge.h"
#include "backend/DataTypes.h"
#include "backend/modbus_client/RegisterConverter.h"
#include <QTimer>
#include <QModbusDataUnit>
#include <QModbusTcpClient>

struct RegisterBlock
{
    QModbusDataUnit::RegisterType type;
    quint16 startAddress;
    QVector<quint16> values;
};

class BACKEND_EXPORT QtModbusBridge : public IModbusBridge
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
    void parseSensorsResponse(const QVector<quint16>& values);
    void parseInfoResponse(const QVector<quint16>& values);
    std::optional<QVector<quint16>> extractValues(QModbusReply* reply, qsizetype expectedSize);
    //OPTIMIZE: move sorting elsewhere and receive regs by reference if encountered speed issues
    void writeRegisterVector(QVector<RegisterBlock> regs);
private:
    ModbusConfig m_cfg;
    QModbusTcpClient m_client;
    QTimer m_pollTimer;
    RegisterConverter m_reg_converter;
    QModbusDevice::State m_prevState = QModbusDevice::UnconnectedState;
};
