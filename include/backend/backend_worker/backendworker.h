#ifndef BACKENDWORKER_H
#define BACKENDWORKER_H

#include <QObject>
#include <QThread>
#include "backend/state_machine/state_machine.h"
#include "backend/DataTypes.h"

//Класс взаимодействия с фронтом
class BackendWorker : public QObject
{
    Q_OBJECT
public:
    explicit BackendWorker(QObject *parent = nullptr);
    ~BackendWorker();

    //Запуск бэкэнда
    void Run();
    //Остановка бэкэнда
    void Stop();

    //Метод для отправки параметров модели бэкэнду
    void SendFrontControlToBackend(FrontControl& control);
    //Запустить обкатку
    void StartEngine();
    //Остановить обкатку
    void StopEngine();

signals:
    //Сигналы для получения(для фронта)
    //Сигнал на приход нового снимка датчиков
    void ReceivedSensorFrame(SensorFrame sensorFrame);
    //Сигнал на приход информации об аварийной остановке
    void ReceivedEmergencyStopInfo();
    //Сигнал на приход фидбека на изменение этапа
    void ReceivedFeedback(DiagState state);
    //Сигнал на приход данных
    void ReceivedData(QVector<Data>& data);
    //Сигнал на приход предупреждений в работе двигателя
    void ReceivedWarn(DiagState state);
    //Сигнал при окончании выполнения этапа обкатки
    void ReceivedStageCompleteInfo();

    //Сигналы для отправки(внутренние)
    void SendedFrontControlToBackend(FrontControl control);
    void StartedEngine();
    void StopedEngine();

private:
    QThread m_machineThread;
    StateMachine* m_machine;
};

#endif // BACKENDWORKER_H
