#include "../../../include/backend/state_machine.h"
#include <iostream>

StateMachine::StateMachine(BackendWorker* backendWorker, QObject *parent) :
    m_communicator(new BackendCommunicator(this)),
    m_modbusBridge(new QtModbusBridge(ConfigData().LoadConfig())), QObject{parent}
{
    connect(backendWorker, &BackendWorker::SendedFrontControlToBackend,
            m_communicator, &BackendCommunicator::ReceivedFrontControl, Qt::QuenedConnection);
    connect(backendWorker, &BackendWorker::SendedModelConfigToBackend,
            m_communicator, &BackendCommunicator::ReceivedModelConfig, Qt::QuenedConnection);

    connect(m_communicator, &BackendCommunicator::SendedSensorFrame,
            backendWorker, &BackendWorker::ReceivedSensorFrame, Qt::QuenedConnection);
    connect(m_communicator, &BackendCommunicator::SendedEmergencyStopInfo,
            backendWorker, &BackendWorker::ReceivedEmergencyStopInfo, Qt::QuenedConnection);
    connect(m_communicator, &BackendCommunicator::SendedFeedback,
            backendWorker, &BackendWorker::ReceivedFeedback, Qt::QuenedConnection);
    connect(m_communicator, &BackendCommunicator::SendedData,
            backendWorker, &BackendWorker::ReceivedData, Qt::QuenedConnection);

    connect(m_communicator, &BackendCommunicator::ReceivedFrontControl,
            this, &StateMachine::onReceivedFrontControl);
    connect(m_communicator, &BackendCommunicator::ReceivedModelConfig,
            this, &StateMachine::onReceivedModelConfig);

    //m_config = config;
    m_stageTimer = new QTimer(this);
}

void StateMachine::Run()
{
    //Здесь должен быть запуск таймера, запуск Modbus клиента
}

void StateMachine::Stop()
{
    //Остановка всех таймеров
}

StateMachine::~StateMachine()
{
    // try{
    //     delete m_stageTimer;
    // }
    // catch (const std::runtime_error& e) {
    //     // Перехватываем исключение
    //     std::cerr << "Ошибка: " << e.what() << std::endl;
    // }
}
