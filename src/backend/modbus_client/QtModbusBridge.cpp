#include "backend/DataTypes.h"
#include "backend/modbus_client/QtModbusBridge.h"
#include <QObject>
#include <QDebug>
#include <QModbusTcpClient>
#include <QVariant>

QtModbusBridge::QtModbusBridge(const ModbusConfig& cfg, QObject* parent) : IModbusBridge(cfg, parent)
{
    m_cfg = cfg;

    m_client.setConnectionParameter(QModbusDevice::NetworkAddressParameter, QVariant::fromValue(m_cfg.host));
    m_client.setConnectionParameter(QModbusDevice::NetworkPortParameter, m_cfg.port);
    m_client.setTimeout(m_cfg.timeoutMs);
    m_client.setNumberOfRetries(m_cfg.retries);

    QObject::connect(&m_pollTimer, &QTimer::timeout, this, &QtModbusBridge::onReadSensors);
    QObject::connect(&m_client, &QModbusDevice::stateChanged, this, &QtModbusBridge::onStateChange);
    QObject::connect(&m_client, &QModbusDevice::errorOccurred, this, &QtModbusBridge::onErrorOccured);

    qDebug() << "[QtModbusBridge] Initialization is successful";

    m_client.connectDevice();
}

QtModbusBridge::~QtModbusBridge()
{
    m_client.disconnectDevice();
}

void QtModbusBridge::onStateChange(QModbusDevice::State state)
{
    // Handling states change
    if (state == QModbusDevice::UnconnectedState)
    {
        qDebug() << "[QtModbusBridge] Modbus client and server are disconnected";
    }
    else if (state == QModbusDevice::ConnectingState)
    {
        qDebug() << "[QtModbusBridge] Connecting to the modbus device...";
    }
    else if (state == QModbusDevice::ConnectedState)
    {
        qDebug() << "[QtModbusBridge] Connected to the modbus server successfully";
    }
    else if (state == QModbusDevice::ClosingState)
    {
        qDebug() << "[QtModbusBridge] Closing the modbus device...";
    }
    // Handling state transitions
    if (state == QModbusDevice::ConnectedState && m_prevState != QModbusDevice::ConnectedState)
    {
        emit connectionRestored();
    }
    else if (m_prevState == QModbusDevice::ConnectedState && state != QModbusDevice::ConnectedState)
    {
        emit connectionLost();
    }
    m_prevState = state;
}

void QtModbusBridge::onErrorOccured()
{
    QModbusDevice::Error error = m_client.error();
    if (error == QModbusDevice::NoError)
        return;

    QString reason = m_client.errorString();
    if (error == QModbusDevice::ConfigurationError)
    {
        emit configurationError(reason);
    }
    else if (error == QModbusDevice::ConnectionError)
    {
        emit connectionError(reason);
    }
    else if (error != QModbusDevice::UnknownError)
    {
        emit requestError(reason);
    }
    else
    {
        emit generalError(reason);
    }
}

void QtModbusBridge::startPolling()
{
    qDebug() << "[QtModbusBridge] Starting to poll server";
    m_pollTimer.start(m_cfg.pollFrequencyMs);
}

void QtModbusBridge::stopPolling()
{
    qDebug() << "[QtModbusBridge] Stopping to poll server";
    m_pollTimer.stop();
}

void QtModbusBridge::onReadSensors()
{
    qDebug() << "[QtModbusBridge] Requesting sensors data";

    QModbusDataUnit dataUnit;
    dataUnit.setRegisterType(QModbusDataUnit::InputRegisters);
    dataUnit.setStartAddress(0);
    dataUnit.setValueCount(InputRegisters::count);

    // Sending request to the device with specified Unit Id
    auto* reply = sendReadRequest(dataUnit);
    if (!reply)
        return;

    // Handling reply with finished signal
    QObject::connect(reply, &QModbusReply::finished, this, [this, reply]()
    {
        auto values = extractValues(reply, InputRegisters::size);
        if (!values.has_value())
            return;
        parseSensorsResponse(values.value());
        reply->deleteLater();
    });
}

void QtModbusBridge::onReadInfo()
{
    qDebug() << "[QtModbusBridge] Requesting model info";

    QModbusDataUnit dataUnit;
    dataUnit.setRegisterType(QModbusDataUnit::HoldingRegisters);
    dataUnit.setStartAddress(0);
    dataUnit.setValueCount(HoldingRegisters::count);

    // Sending request to the device with specified Unit Id
    auto* reply = sendReadRequest(dataUnit);
    if (!reply)
        return;

    // Handling reply with finished signal
    QObject::connect(reply, &QModbusReply::finished, this, [this, reply]()
    {
        auto values = extractValues(reply, HoldingRegisters::size);
        if (!values.has_value())
            return;
        parseInfoResponse(values.value());
        reply->deleteLater();
    });
}

void QtModbusBridge::onWriteConfig(const ModelConfig& cmd)
{
    qDebug() << "[QtModbusBridge] Writing a new configuration to the model";
    // TODO: implement later
}

void QtModbusBridge::onWriteDecision(const Decision& decision)
{
    qDebug() << "[QtModbusBridge] Sending a decision to the model";
    // TODO: implement later
}

std::optional<QVector<quint16>> QtModbusBridge::extractValues(QModbusReply* reply, qsizetype expectedSize)
{
    if (reply->error() != QModbusDevice::NoError)
        return std::nullopt;

    const QModbusDataUnit result = reply->result();
    const QVector<quint16> values = result.values();

    if (values.size() != expectedSize)
    {
        emit requestError("Size of received data differs from expected HoldingRegisters::size");
        return std::nullopt;
    }

    return values;
}

void QtModbusBridge::parseSensorsResponse(const QVector<quint16>& values)
{
    SensorFrame frame = {
        static_cast<double>(values[InputRegisters::T_cool]),
        static_cast<double>(values[InputRegisters::T_AD]),
        static_cast<double>(values[InputRegisters::T_ballast]),
        static_cast<double>(values[InputRegisters::P_oil]),
        static_cast<double>(values[InputRegisters::M_AD]),
        static_cast<double>(values[InputRegisters::f_AD]),
        static_cast<qint64>(values[InputRegisters::timestamp_ir]),
        1  // TODO: clarify what does stage mean
    };
    emit sensorsDataReady(frame);
}

void QtModbusBridge::parseInfoResponse(const QVector<quint16>& values)
{
    ModelConfig info = {
        static_cast<double>(values[HoldingRegisters::T_cool_max]),
        static_cast<double>(values[HoldingRegisters::T_AD_max]),
        static_cast<double>(values[HoldingRegisters::T_ballast_max]),
        static_cast<double>(values[HoldingRegisters::P_oil_max]),
        static_cast<double>(values[HoldingRegisters::P_oil_min]),
        static_cast<int>(values[HoldingRegisters::omega_ICE_max_prir]),
        static_cast<int>(values[HoldingRegisters::omega_ICE_max_run]),
    };
    emit modelInfoReady(info);
}

QModbusReply* QtModbusBridge::sendWriteRequest(const QModbusDataUnit& dataUnit) {
    auto* reply = m_client.sendWriteRequest(dataUnit, m_cfg.unitId);
    if (!reply)
    {
        return nullptr;
    }
    // Check if request failed immediately
    if (reply->isFinished())
    {
        reply->deleteLater();
        return nullptr;
    }
    return reply;
}

QModbusReply* QtModbusBridge::sendReadRequest(const QModbusDataUnit& dataUnit) {
    auto* reply = m_client.sendReadRequest(dataUnit, m_cfg.unitId);
    if (!reply)
    {
        return nullptr;
    }
    // Check if request failed immediately
    if (reply->isFinished())
    {
        reply->deleteLater();
        return nullptr;
    }
    return reply;
}