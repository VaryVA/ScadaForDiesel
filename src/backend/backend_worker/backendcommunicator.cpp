#include "backend/backend_worker/backendcommunicator.h"

BackendCommunicator::BackendCommunicator(QObject *parent)
    : QObject{parent} {}

void BackendCommunicator::SendSensorFrameToFrontend(SensorFrame& sensorFrame)
{
    emit SendedSensorFrame(sensorFrame);
}

void BackendCommunicator::SendEmergencyStopInfoToFrontend()
{
    emit SendedEmergencyStopInfo();
}

void BackendCommunicator::SendFeedbackToFrontend(bool isComplete)
{
    emit SendedFeedback(isComplete);
}

void BackendCommunicator::SendDataToFrontend(QVector<Data>& data)
{
    emit SendedData(data);
}
