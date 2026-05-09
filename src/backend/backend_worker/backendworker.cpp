#include "backend/backend_worker/backendworker.h"
#include "backend/state_machine/state_machine.h"

BackendWorker::BackendWorker(QObject* parent) :
    m_machine(new StateMachine(this, nullptr)), QObject{parent}
{
    qRegisterMetaType<FrontControl>();
    qRegisterMetaType<ModelConfig>();
    qRegisterMetaType<SensorFrame>();
    connect(&m_machineThread, &QThread::started, m_machine, &StateMachine::start, Qt::QueuedConnection);
    connect(&m_machineThread, &QThread::finished, m_machine, &StateMachine::stop, Qt::QueuedConnection);
}

void BackendWorker::SendFrontControlToBackend(FrontControl& control)
{
    emit SendedFrontControlToBackend(control);
}

void BackendWorker::StartEngine()
{
    emit StartedEngine();
}

void BackendWorker::StopEngine()
{
    emit StopedEngine();
}

void BackendWorker::Run()
{
    m_machine->moveToThread(&m_machineThread);
    m_machineThread.start();
}

void BackendWorker::Stop()
{
    m_machineThread.quit();
}

BackendWorker::~BackendWorker()
{
    if(m_machineThread.isRunning())
        m_machineThread.quit();
    m_machineThread.wait();
    m_machine->deleteLater();
}
