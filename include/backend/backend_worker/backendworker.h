#ifndef BACKENDWORKER_H
#define BACKENDWORKER_H

#include <QObject>
#include <QThread>
#include "../state_machine.h"
#include "../DataTypes.h"

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

    //Метод для отправки команды на изменение состояния модели бэкэнду
    void SendFrontControlToBackend(FrontControl& control);
    //Метод для отправки нового кофига модели бэкэнду
    void SendModelConfigToBackend(ModelConfig& config);

signals:
    //Сигналы для получения(для фронта)
    //Сигнал на приход нового снимка датчиков
    void ReceivedSensorFrame(SensorFrame sensorFrame);
    //Сигнал на приход информации об аварийной остановке
    void ReceivedEmergencyStopInfo();
    //Сигнал на приход фидбека на изменение этапа
    void ReceivedFeedback(bool isComplete);
    //Сигнал на приход данных
    void ReceivedData(QVector<Data>& data);

    //Сигналы для отправки(внутренние)
    void SendedFrontControlToBackend(FrontControl control);
    void SendedModelConfigToBackend(ModelConfig config);

private:
    QThread m_machineThread;
    StateMachine* m_machine;
};

#endif // BACKENDWORKER_H
