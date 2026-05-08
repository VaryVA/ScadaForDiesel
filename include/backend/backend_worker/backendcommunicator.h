#ifndef BACKENDCOMMUNICATOR_H
#define BACKENDCOMMUNICATOR_H

#include <QObject>
#include "backend/DataTypes.h"

class BackendCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit BackendCommunicator(QObject *parent = nullptr);

    //Метод для отправки нового снимка датчиков фронту
    void SendSensorFrameToFrontend(SensorFrame& sensorFrame);
    //Метод для отправки информации фронту об аварийной остановке
    void SendEmergencyStopInfoToFrontend();
    //Метод для отправки фидбека фронту на операцию
    void SendFeedbackToFrontend(DiagState state);
    //Метод для отправки данных фронту
    void SendDataToFrontend(QVector<Data>& data);
    //Метод для отправки предупреждений фронту
    void SendWarnToFrontend(DiagState state);

signals:
    //Сигналы для отправки(внутренние)
    void SendedSensorFrame(SensorFrame& sensorFrame);
    void SendedEmergencyStopInfo();
    void SendedFeedback(DiagState state);
    void SendedData(QVector<Data>& data);
    void SendedWarnToFrontend(DiagState state);

    //Сигналы для получения
    //Сигнал на приход запроса на изменения этапа эксперимента
    void ReceivedFrontControl(FrontControl control);
    //Сигнал на запуск обкатки
    void ReceivedStartEngine();
    //Сигнал на остановку обкатки
    void ReceivedStopEngine();
};

#endif // BACKENDCOMMUNICATOR_H
