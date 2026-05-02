#ifndef BACKENDWORKER_H
#define BACKENDWORKER_H

#include <QObject>
#include <QThread>
#include "../backendcore.h"
#include "../struct.h"

//Класс взаимодействия с фронтом
class BackendWorker : public QObject
{
    Q_OBJECT
public:
    explicit BackendWorker(QObject *parent = nullptr);
    ~BackendWorker();

    //Запуск ядра бэкэнда
    void RunCore();
    //Остановка ядра бэкэнда
    void StopCore();

    //Команда бэкэнду от фронта
    void CommandToBackend(CommandToBackend command);

signals:
    //Сигнал прихода новых данных для фронта
    void NewSensorFrame(SensorFrame sensorFrame);
    //Сигнал возникновения аварийной остановки для фронта
    void EmergencyStop();

private:
    QThread m_coreThread;
    BackendCore* m_backendCore;
};

#endif // BACKENDWORKER_H
