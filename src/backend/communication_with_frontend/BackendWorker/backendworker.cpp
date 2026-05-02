#include "backendworker.h"

BackendWorker::BackendWorker(QObject *parent) :
    m_backendCore(new BackendCore()), m_coreThread(QThread(this)), QObject{parent}
{

}

void BackendWorker::RunCore()
{
    auto backendCommunicator = m_backendCore->GetBackendCommunicator();
    connect(&backendCommunicator, &BackendCommunicator::NewSensorFrame, this, &BackendWorker::NewSensorFrame, Qt::QueuedConnection);
    connect(&backendCommunicator, &BackendCommunicator::EmergencyStop, this, &BackendWorker::EmergencyStop, Qt::QueuedConnection);
    connect(&m_coreThread, &QThread::started, m_backendCore, &BackendCore::RunCore, Qt::QueuedConnection);
    connect(&m_coreThread, &QThread::finished, m_backendCore, &BackendCore::StopCore, Qt::QueuedConnection);
    m_backendCore->moveToThread(&m_coreThread);
    m_coreThread.start();
}

void BackendWorker::StopCore()
{
    m_coreThread.quit();
}

BackendWorker::~BackendWorker()
{
    m_coreThread.deleteLater();
}
