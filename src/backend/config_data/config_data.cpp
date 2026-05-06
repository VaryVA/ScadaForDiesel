#include "../../../include/backend/config_data.h"

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
        switch(partLine)
        {
            case m_HOST:
                config.host = value;
                break;
            case m_PORT:
                config.port = value;
                break;
            case m_POLL_FREQ:
                config.pollFrequencyMs = value;
                break;
            case m_TIMEOUT:
                config.timeoutMs = value;
                break;
            case m_RETRIES:
                config.retries = value;
                break;
            case m_UNIT_ID:
                config.unitId = value;
                break;
        }
    }

    file.close();
    return config;
}
