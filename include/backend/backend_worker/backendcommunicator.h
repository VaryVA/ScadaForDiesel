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
    //Метод для отправки фидбека фронту на изменение этапа
    void SendFeedbackToFrontend(bool isComplete);
    //Метод для отправки данных фронту
    void SendDataToFrontend(QVector<Data>& data);

signals:
    //Сигналы для отправки
    void SendedSensorFrame(SensorFrame& sensorFrame);
    void SendedEmergencyStopInfo();
    void SendedFeedback(bool isComplete);
    void SendedData(QVector<Data>& data);

    //Сигналы для получения
    //Сигнал на приход нового конфига модели от фронта
    void ReceivedModelConfig(ModelConfig config);
    //Сигнал на приход запроса на изменения этапа эксперимента
    void ReceivedFrontControl(FrontControl control);
};

#endif // BACKENDCOMMUNICATOR_H
