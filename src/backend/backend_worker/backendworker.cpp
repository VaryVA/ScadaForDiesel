#include "../../../include/backend/backend_worker/backendworker.h"
#include "../../../include/backend/state_machine.h"

BackendWorker::BackendWorker(QObject* parent) :
    m_machine(new StateMachine(this, parent)), m_machineThread(QThread(this)), QObject{parent}
{
    qRegisterMetaType<FrontControl>();
    qRegisterMetaType<ModelConfig>();
    qRegisterMetaType<SensorFrame>();
    connect(&m_machineThread, &QThread::started, m_machine, &StateMachine::Run, Qt::QueuedConnection);
    connect(&m_machineThread, &QThread::finished, m_machine, &StateMachine::Stop, Qt::QueuedConnection);
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
    m_machine->deleteLater();
}
