#include "backend/DataTypes.h"
#include "backend/modbus_client/QtModbusBridge.h"
#include <QObject>
#include <QDebug>
#include <QModbusTcpClient>
#include <QVariant>
#include <algorithm>

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
    {
        return;
    }

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
    {
        return;
    }

    // Handling reply with finished signal
    QObject::connect(reply, &QModbusReply::finished, this, [this, reply]()
    {
        qDebug() << "[QtModbusBridge] Received reply with sensor data, begin to extract values";
        auto values = extractValues(reply, InputRegisters::count);
        if (!values.has_value())
        {
            qWarning() << "[QtModbusBridge] Failed to extract sensor data";
            return;
        }
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
    {
        return;
    }

    // Handling reply with finished signal
    QObject::connect(reply, &QModbusReply::finished, this, [this, reply]()
    {
        qDebug() << "[QtModbusBridge] Received reply with model info data, begin to extract values";
        auto values = extractValues(reply, HoldingRegisters::count);
        if (!values.has_value())
        {
            qWarning() << "[QtModbusBridge] Failed to extract model info data";
            return;
        }
        parseInfoResponse(values.value());
        reply->deleteLater();
    });
}

void QtModbusBridge::onWriteConfig(const ModelConfig& cmd)
{
    qDebug() << "[QtModbusBridge] Writing a new configuration to the model";

    QVector<RegisterBlock> regs;
    regs.reserve(9);

    regs.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        HoldingRegisters::omega_ICE_max_prir,
        m_reg_converter.toRegisterWords(cmd.maxRpmRun)
    );
    regs.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        HoldingRegisters::omega_ICE_max_run,
        m_reg_converter.toRegisterWords(cmd.maxDieselPressure)
    );
    regs.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        HoldingRegisters::P_oil_max,
        m_reg_converter.toRegisterWords(cmd.maxDieselPressure)
    );
    regs.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        HoldingRegisters::P_oil_min,
        m_reg_converter.toRegisterWords(cmd.minDieselPressure)
    );
    regs.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        HoldingRegisters::T_cool_max,
        m_reg_converter.toRegisterWords(cmd.maxDieselTemp)
    );
    regs.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        HoldingRegisters::T_AD_max,
        m_reg_converter.toRegisterWords(cmd.maxMotorTemp)
    );
    regs.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        HoldingRegisters::T_ballast_max,
        m_reg_converter.toRegisterWords(cmd.maxResistorTemp)
    );
    regs.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        HoldingRegisters::f_AD_Input,
        m_reg_converter.toRegisterWords(cmd.freqAD)
    );
    regs.emplace_back(
        QModbusDataUnit::HoldingRegisters,
        HoldingRegisters::M_AD_target,
        m_reg_converter.toRegisterWords(cmd.momentAD)
    );
    writeRegisterVector(regs);
}

void QtModbusBridge::onWriteDecision(const Decision& decision)
{
    qDebug() << "[QtModbusBridge] Sending a decision to the model";
    QVector<RegisterBlock> regs;
    regs.reserve(decision.controls.size());
    for (const auto& control : decision.controls)
    {
        QModbusDataUnit::RegisterType regType = QModbusDataUnit::Invalid;
        qsizetype startAddress = -1;
        switch(control.type)
        {
        case ControlType::SimulationCommand:
            regType = QModbusDataUnit::HoldingRegisters;
            startAddress = HoldingRegisters::simulationCommand;
            break;
        case ControlType::SimulationMode:
            regType = QModbusDataUnit::HoldingRegisters;
            startAddress = HoldingRegisters::simulationMode;
            break;
        case ControlType::SimulationRequest:
            regType = QModbusDataUnit::HoldingRegisters;
            startAddress = HoldingRegisters::simulationRequest;
            break;
        case ControlType::Fan_AD:
            regType = QModbusDataUnit::Coils;
            startAddress = CoilsRegisters::fan_AD;
            break;
        case ControlType::Fan_Ballast:
            regType = QModbusDataUnit::Coils;
            startAddress = CoilsRegisters::fan_ballast;
            break;
        case ControlType::Fan_ICE:
            regType = QModbusDataUnit::Coils;
            startAddress = CoilsRegisters::fan_ICE;
            break;
        case ControlType::None:
            break;
        default:
            qWarning() << "[QtModbusBridge] Unexpected ControlType was received in onWriteDecision";
        }
        if (startAddress == -1)
            continue;
        regs.emplace_back(
            regType,
            startAddress,
            QVector<quint16>{ static_cast<quint16>(control.value) }
        );
    }
    writeRegisterVector(regs);
}

std::optional<QVector<quint16>> QtModbusBridge::extractValues(QModbusReply* reply, qsizetype expectedSize)
{
    if (reply->error() != QModbusDevice::NoError)
    {
        return std::nullopt;
    }

    const QModbusDataUnit result = reply->result();
    const QVector<quint16> values = result.values();

    if (values.size() != expectedSize)
    {
        qWarning() << "[extractValues] size mismatch, values.size() =" << values.size()
                    << ", expectedSize =" << expectedSize;
        emit requestError("Size of received data differs from expected size");
        return std::nullopt;
    }

    return values;
}

void QtModbusBridge::parseSensorsResponse(const QVector<quint16>& values)
{
    qDebug() << "[parseSensorsResponse] entered, values.size() =" << values.size();

    SensorFrame frame = {
        m_reg_converter.fromRegisterWordDouble(&values[InputRegisters::T_cool]),
        m_reg_converter.fromRegisterWordDouble(&values[InputRegisters::T_AD]),
        m_reg_converter.fromRegisterWordDouble(&values[InputRegisters::T_ballast]),
        m_reg_converter.fromRegisterWordDouble(&values[InputRegisters::P_oil]),
        m_reg_converter.fromRegisterWordDouble(&values[InputRegisters::M_AD]),
        m_reg_converter.fromRegisterWordDouble(&values[InputRegisters::f_AD]),
        m_reg_converter.fromRegisterWordQint(&values[InputRegisters::timestamp_ir]),
        1  // TODO: clarify what does stage mean
    };
    emit sensorsDataReady(frame);
}

void QtModbusBridge::parseInfoResponse(const QVector<quint16>& values)
{
    ModelConfig info = {
        m_reg_converter.fromRegisterWordDouble(&values[HoldingRegisters::T_cool_max]),
        m_reg_converter.fromRegisterWordDouble(&values[HoldingRegisters::T_AD_max]),
        m_reg_converter.fromRegisterWordDouble(&values[HoldingRegisters::T_ballast_max]),
        m_reg_converter.fromRegisterWordDouble(&values[HoldingRegisters::P_oil_max]),
        m_reg_converter.fromRegisterWordDouble(&values[HoldingRegisters::P_oil_min]),
        m_reg_converter.fromRegisterWordInt(&values[HoldingRegisters::omega_ICE_max_prir]),
        m_reg_converter.fromRegisterWordInt(&values[HoldingRegisters::omega_ICE_max_run]),
    };
    emit modelInfoReady(info);
}

void QtModbusBridge::writeRegisterVector(QVector<RegisterBlock> regs)
{
    if (regs.empty())
        return;

    std::sort(regs.begin(), regs.end(), [](const auto& lhs, const auto& rhs) {
        return std::tie(lhs.type, lhs.startAddress) < std::tie(rhs.type, rhs.startAddress);
    });

    QModbusDataUnit::RegisterType batchType = regs[0].type;
    quint16 batchStart = regs[0].startAddress;
    quint16 expectedAddress = batchStart;

    QVector<quint16> batchValues;
    batchValues.reserve(regs.size());

    auto flush = [&]()
    {
        if (batchValues.isEmpty())
            return;

        QModbusDataUnit unit(
            batchType,
            batchStart,
            batchValues
        );

        m_client.sendWriteRequest(unit, m_cfg.unitId);
    };

    for (const RegisterBlock& reg : regs)
    {
        if (reg.startAddress != expectedAddress || reg.type != batchType)
        {
            flush();

            batchType = reg.type;
            batchStart = reg.startAddress;
            batchValues.clear();
            expectedAddress = reg.startAddress;
        }

        batchValues.append(reg.values);
        expectedAddress += reg.values.size();
    }

    flush();
}