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

void BackendCommunicator::SendFeedbackToFrontend(DiagState state)
{
    emit SendedFeedback(state);
}

void BackendCommunicator::SendDataToFrontend(QVector<Data>& data)
{
    emit SendedData(data);
}

void BackendCommunicator::SendWarnToFrontend(DiagState state)
{
    emit SendedWarnToFrontend(state);
}

void BackendCommunicator::SendStageCompleteInfoToFrontend()
{
    emit SendedStageCompleteInfoToFrontend();
}
