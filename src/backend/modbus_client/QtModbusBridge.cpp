#include "backend/DataTypes.h"
#include "backend/modbus_client/QtModbusBridge.h"
#include <QObject>
#include <QDebug>
#include <QModbusTcpClient>
#include <QVariant>
#include <memory.h>

QtModbusBridge::QtModbusBridge(const ModbusConfig& cfg, QObject* parent) : IModbusBridge(cfg, parent)
{
    m_cfg = cfg;

    m_client.setConnectionParameter(QModbusDevice::NetworkAddressParameter, QVariant::fromValue(m_cfg.host));
    m_client.setConnectionParameter(QModbusDevice::NetworkPortParameter, m_cfg.port);
    m_client.setTimeout(m_cfg.timeoutMs);
    m_client.setNumberOfRetries(m_cfg.retries);

    QObject::connect(&m_pollTimer, &QTimer::timeout, this, &QtModbusBridge::requestSensors);

    QObject::connect(&m_client, &QModbusDevice::stateChanged, this, [this](QModbusDevice::State state)
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
            qDebug() << "[QtModbusBridge] Connected to the modbus server succesfully";
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
    });

    QObject::connect(&m_client, &QModbusDevice::errorOccurred, this, [this]()
    {
        QModbusDevice::Error error = m_client.error();
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
    });

    qDebug() << "[QtModbusBridge] Initialization is successfull";

    m_client.connectDevice();
}

QtModbusBridge::~QtModbusBridge()
{
    m_client.disconnectDevice();
}

void QtModbusBridge::startPolling()
{
    qDebug() << "[QtModbusBridge] Starting to poll server";
    m_pollTimer.start(m_cfg.pollFrequencyMs);
}

void QtModbusBridge::stopPolling()
{
    qDebug() << "[QtModbusBridge] Starting to poll server";
    m_pollTimer.stop();
}

void QtModbusBridge::onReadSensors()
{
    requestSensors();
}

void QtModbusBridge::onReadInfo()
{
    requestInfo();
}

void QtModbusBridge::onWriteConfig(const ModelConfig& cmd)
{
    qDebug() << "[QtModbusBridge] Requesting sensors data";

    m_dataUnit.setRegisterType(QModbusDataUnit::InputRegisters);
    m_dataUnit.setStartAddress(0);
    m_dataUnit.setValueCount(sizeof(m_cfg.input) / 2);

    // Sending request to the device with specified Unit Id
    auto *reply = m_client.sendReadRequest(m_dataUnit, m_cfg.unitId);

    if (!reply)
    {
        return;
    }

    // Check if request failed immediately
    if (reply->isFinished())
    {
        reply->deleteLater();
        return;
    }

    // Handling reply with finished signal
    QObject::connect(reply, &QModbusReply::finished, this, [this, reply]()
    {
        parseSensorsResponse(reply);
        reply->deleteLater();
    });
}

void QtModbusBridge::onWriteDecision(const Decision& decision)
{

}

void QtModbusBridge::parseSensorsResponse(QModbusReply* reply)
{
    if (reply->error() == QModbusDevice::NoError)
    {
        return;
    }

    const QModbusDataUnit result = reply->result();
    const QVector<quint16> values = result.values();

    qDebug() << "[QtModbusBridge] Read registers:" << values.size();
    for (int i = 0; i < values.size(); ++i)
    {
        qDebug() << "[QtModbusBridge] Register #" << i << ":" << values[i];
    }
}

void QtModbusBridge::requestSensors()
{
    qDebug() << "[QtModbusBridge] Requesting sensors data";

    m_dataUnit.setRegisterType(QModbusDataUnit::InputRegisters);
    m_dataUnit.setStartAddress(0);
    m_dataUnit.setValueCount(sizeof(m_cfg.input) / 2);

    // Sending request to the device with specified Unit Id
    auto *reply = m_client.sendReadRequest(m_dataUnit, m_cfg.unitId);

    if (!reply)
    {
        return;
    }

    // Check if request failed immediately
    if (reply->isFinished())
    {
        reply->deleteLater();
        return;
    }

    // Handling reply with finished signal
    QObject::connect(reply, &QModbusReply::finished, this, [this, reply]()
    {
        parseSensorsResponse(reply);
        reply->deleteLater();
    });
}

void QtModbusBridge::parseInfoResponse(QModbusReply* reply)
{
    if (reply->error() == QModbusDevice::NoError)
    {
        return;
    }

    const QModbusDataUnit result = reply->result();
    const QVector<quint16> values = result.values();

    qDebug() << "[QtModbusBridge] Read registers:" << values.size();
    for (int i = 0; i < values.size(); ++i)
    {
        qDebug() << "[QtModbusBridge] Register #" << i << ":" << values[i];
    }
}

void QtModbusBridge::requestInfo()
{
    qDebug() << "[QtModbusBridge] Requesting model info";

    m_dataUnit.setRegisterType(QModbusDataUnit::HoldingRegisters);
    m_dataUnit.setStartAddress(0);
    m_dataUnit.setValueCount(sizeof(m_cfg.holding) / 2);

    // Sending request to the device with specified Unit Id
    auto *reply = m_client.sendReadRequest(m_dataUnit, m_cfg.unitId);

    if (!reply)
    {
        return;
    }

    // Check if request failed immediately
    if (reply->isFinished())
    {
        reply->deleteLater();
        return;
    }

    // Handling reply with finished signal
    QObject::connect(reply, &QModbusReply::finished, this, [this, reply]()
    {
        parseInfoResponse(reply);
        reply->deleteLater();
    });
}