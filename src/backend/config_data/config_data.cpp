#include "backend/config_data/config_data.h"

ConfigData::ConfigData(){}

ModbusConfig ConfigData::LoadConfig()
{
    QFile file(m_FILENAME);
    if(!file.exists())
    {
        qInfo("Файл конфигурации бэкэнда отсутствует. Будут загружены параметры по умолчанию");
        return ModbusConfig();
    }

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text));
    {
        qInfo("Файл конфигурации защищен от чтения. Будут загружены параметры по умолчанию");
        return ModbusConfig();
    }

    ModbusConfig config;
    while(!file.atEnd())
    {
        QString line = file.readLine();
        QStringList partLine = line.split('=');
        if(partLine.count() != 2)
            continue;
        QString& value = partLine[1];
        if(m_HOST == value)
        {
            config.host = value;
            continue;
        }
        if(m_PORT == value)
        {
            config.port = value.toUInt();
            continue;
        }
        if(m_POLL_FREQ == value)
        {
            config.pollFrequencyMs = value.toInt();
            continue;
        }
        if(m_TIMEOUT == value)
        {
            config.timeoutMs = value.toInt();
            continue;
        }
        if(m_RETRIES == value)
        {
            config.retries = value.toInt();
            continue;
        }
        if(m_UNIT_ID == value)
            config.unitId = value.toInt();
    }

    file.close();
    return config;
}
