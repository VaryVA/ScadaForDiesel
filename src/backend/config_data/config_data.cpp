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

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qInfo("Файл конфигурации защищен от чтения. Будут загружены параметры по умолчанию");
        return ModbusConfig();
    }

    ModbusConfig config;
    while(!file.atEnd())
    {
        QString line = file.readLine();
        line.chop(1);
        QStringList partLine = line.split('=');
        if(partLine.count() != 2)
            continue;
        QString& key = partLine[0];
        QString& value = partLine[1];
        if(m_HOST == key)
        {
            config.host = value;
            continue;
        }
        if(m_PORT == key)
        {
            config.port = value.toUInt();
            continue;
        }
        if(m_POLL_FREQ == key)
        {
            config.pollFrequencyMs = value.toInt();
            continue;
        }
        if(m_TIMEOUT == key)
        {
            config.timeoutMs = value.toInt();
            continue;
        }
        if(m_RETRIES == key)
        {
            config.retries = value.toInt();
            continue;
        }
        if(m_UNIT_ID == key)
            config.unitId = value.toInt();
    }

    file.close();
    return config;
}
