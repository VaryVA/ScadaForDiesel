#ifndef CONFIG_DATA_H
#define CONFIG_DATA_H

#include <QObject>
#include <QFile>
#include <QString>
#include <QHash>
#include "backend/DataTypes.h"

class ConfigData : public QObject
{
public:
    ConfigData();
    ModbusConfig LoadConfig();

private:
    const QString m_HOST = "host";
    const QString m_PORT = "port";
    const QString m_POLL_FREQ = "poll_freq_ms";
    const QString m_TIMEOUT = "timeout_ms";
    const QString m_RETRIES = "retries";
    const QString m_UNIT_ID = "unit_id";

    const QString m_FILENAME = "configBackend.txt";
};



#endif //Config_data H
