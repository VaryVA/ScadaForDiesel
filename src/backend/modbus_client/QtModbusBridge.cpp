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

    QModbusDataUnit dataUnit(QModbusDataUnit::InputRegisters, 0, InputRegisters::count);

    // Sending request to the device with specified Unit Id
    auto* reply = m_client.sendReadRequest(dataUnit, m_cfg.unitId);
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

    QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, 0, HoldingRegisters::count);

    // Sending request to the device with specified Unit Id
    auto* reply = m_client.sendReadRequest(dataUnit, m_cfg.unitId);
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

    QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, 0, HoldingRegisters::count);
    setRegisterValues(dataUnit, HoldingRegisters::omega_ICE_max_prir, m_decoder.toRegisterWords(cmd.maxRpmPrir));
    setRegisterValues(dataUnit, HoldingRegisters::omega_ICE_max_run, m_decoder.toRegisterWords(cmd.maxRpmRun));
    setRegisterValues(dataUnit, HoldingRegisters::P_oil_max, m_decoder.toRegisterWords(cmd.maxDieselPressure));
    setRegisterValues(dataUnit, HoldingRegisters::P_oil_min, m_decoder.toRegisterWords(cmd.minDieselPressure));
    setRegisterValues(dataUnit, HoldingRegisters::T_cool_max, m_decoder.toRegisterWords(cmd.maxDieselTemp));
    setRegisterValues(dataUnit, HoldingRegisters::T_AD_max, m_decoder.toRegisterWords(cmd.maxMotorTemp));
    setRegisterValues(dataUnit, HoldingRegisters::T_ballast_max, m_decoder.toRegisterWords(cmd.maxResistorTemp));

    auto* reply = m_client.sendWriteRequest(dataUnit, m_cfg.unitId);
    if (!reply)
        return;
}

void QtModbusBridge::onWriteDecision(const Decision& decision)
{
    qDebug() << "[QtModbusBridge] Sending a decision to the model";
    QModbusDataUnit dataUnit(QModbusDataUnit::HoldingRegisters, 0, HoldingRegisters::count);
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
        m_decoder.fromRegisterWordDouble(&values[InputRegisters::T_cool]),
        m_decoder.fromRegisterWordDouble(&values[InputRegisters::T_AD]),
        m_decoder.fromRegisterWordDouble(&values[InputRegisters::T_ballast]),
        m_decoder.fromRegisterWordDouble(&values[InputRegisters::P_oil]),
        m_decoder.fromRegisterWordDouble(&values[InputRegisters::M_AD]),
        m_decoder.fromRegisterWordDouble(&values[InputRegisters::f_AD]),
        m_decoder.fromRegisterWordQint(&values[InputRegisters::timestamp_ir]),
        1  // TODO: clarify what does stage mean
    };
    emit sensorsDataReady(frame);
}

void QtModbusBridge::parseInfoResponse(const QVector<quint16>& values)
{
    ModelConfig info = {
        m_decoder.fromRegisterWordDouble(&values[HoldingRegisters::T_cool_max]),
        m_decoder.fromRegisterWordDouble(&values[HoldingRegisters::T_AD_max]),
        m_decoder.fromRegisterWordDouble(&values[HoldingRegisters::T_ballast_max]),
        m_decoder.fromRegisterWordDouble(&values[HoldingRegisters::P_oil_max]),
        m_decoder.fromRegisterWordDouble(&values[HoldingRegisters::P_oil_min]),
        m_decoder.fromRegisterWordInt(&values[HoldingRegisters::omega_ICE_max_prir]),
        m_decoder.fromRegisterWordInt(&values[HoldingRegisters::omega_ICE_max_run]),
    };
    emit modelInfoReady(info);
}

void QtModbusBridge::setRegisterValues(QModbusDataUnit& dataUnit, qsizetype startAddress, const QList<quint16>& values)
{
    for (qsizetype i = 0; i < values.size(); ++i)
    {
        dataUnit.setValue(startAddress + i, values[i]);
    }
}