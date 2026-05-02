#ifndef BACKENDCOMMUNICATOR_H
#define BACKENDCOMMUNICATOR_H

#include <QObject>
#include "../struct.h"

class BackendCommunicator : public QObject
{
    Q_OBJECT
public:
    explicit BackendCommunicator(QObject *parent = nullptr);

    //Метод для отправки новых данных фронту
    void SendSensorFrameToFrontend(SensorFrame& sensorFrame);
    //Метод для отправки информации фронту об аварийной остановке
    void SendEmergencyStopInfoToFrontend();

signals:
    //Сигнал отправки новых данных фронту
    void NewSensorFrame(SensorFrame sensorFrame);
    //Сигнал отправки информации фронту об аварийной остановке
    void EmergencyStop();
    //Сигнал на комманду от фронта
    void CommandToBackend(CommandToBackend command);
private slots:
    //Слот обработки комманды от фронта
    void onCommandToBackend(CommandToBackend command);
};

#endif // BACKENDCOMMUNICATOR_H
