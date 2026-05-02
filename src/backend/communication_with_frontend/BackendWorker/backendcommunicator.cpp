#include "backendcommunicator.h"

BackendCommunicator::BackendCommunicator(QObject *parent)
    : QObject{parent}
{
    connect(this, &BackendCommunicator::CommandToBackend, &BackendCommunicator::onCommandToBackend);
}

void BackendCommunicator::SendSensorFrameToFrontend(SensorFrame& sensorFrame)
{
    emit NewSensorFrame(sensorFrame);
}

void BackendCommunicator::SendEmergencyStopInfoToFrontend()
{
    emit EmergencyStop();
}

void BackendCommunicator::onCommandToBackend(CommandToBackend command)
{
    //Отправка команды в модуль бизнес-логики
}
