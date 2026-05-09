#include "backend/state_machine/state_machine.h"
#include "backend/backend_worker/backendworker.h"
#include "backend/modbus_client/MockModbusBridge.h"
#include "backend/data_store/DataStore.h"
#include <QDateTime>

StateMachine::StateMachine(BackendWorker* backendWorker, QObject* parent)
    : QObject(parent)
    , m_communicator(new BackendCommunicator(this))
    , m_pollTimer(new QTimer(this))
    , m_stageTimer(new QTimer(this))
{
    ModbusConfig modbusCfg = ConfigData().LoadConfig();
    m_modbusBridge = new MockModbusBridge(modbusCfg, this);

    // ----- Подготавливаем CSV-коннекторы для хранилища -----
    // пути можно вынести в настройки, здесь для примера
    m_Connector = new FileConnector("measurements.csv", SIZE_FILE_STR);

    // ----- Создаём DataStore -----
    m_dataStore = new DataStore(m_Connector, this);

    m_dataProcessor = new DataProcessor(ModelConfig{}, this);

    //Коннекты для коммуникатора
    connect(backendWorker, &BackendWorker::SendedFrontControlToBackend,
            m_communicator, &BackendCommunicator::ReceivedFrontControl, Qt::QueuedConnection);
    connect(backendWorker, &BackendWorker::StartedEngine,
            m_communicator, &BackendCommunicator::ReceivedStartEngine, Qt::QueuedConnection);
    connect(backendWorker, &BackendWorker::StopedEngine,
            m_communicator, &BackendCommunicator::ReceivedStopEngine, Qt::QueuedConnection);

    connect(m_communicator, &BackendCommunicator::SendedSensorFrame,
            backendWorker, &BackendWorker::ReceivedSensorFrame, Qt::QueuedConnection);
    connect(m_communicator, &BackendCommunicator::SendedEmergencyStopInfo,
            backendWorker, &BackendWorker::ReceivedEmergencyStopInfo, Qt::QueuedConnection);
    connect(m_communicator, &BackendCommunicator::SendedFeedback,
            backendWorker, &BackendWorker::ReceivedFeedback, Qt::QueuedConnection);
    connect(m_communicator, &BackendCommunicator::SendedData,
            backendWorker, &BackendWorker::ReceivedData, Qt::QueuedConnection);
    connect(m_communicator, &BackendCommunicator::SendedWarnToFrontend,
            backendWorker, &BackendWorker::ReceivedWarn, Qt::QueuedConnection);
    connect(m_communicator, &BackendCommunicator::SendedStageCompleteInfoToFrontend,
            backendWorker, &BackendWorker::ReceivedStageCompleteInfo, Qt::QueuedConnection);

    connect(m_communicator, &BackendCommunicator::ReceivedFrontControl,
            this, &StateMachine::onReceivedFrontControl);

    connect(m_modbusBridge, &IModbusBridge::sensorsDataReady,
            this, &StateMachine::onSensorsDataReady);
    connect(m_modbusBridge, &IModbusBridge::configurationError,
            this, &StateMachine::onModbusConfigurationError);
    connect(m_modbusBridge, &IModbusBridge::connectionError,
            this, &StateMachine::onModbusConnectionError);
    connect(m_modbusBridge, &IModbusBridge::requestError,
            this, &StateMachine::onModbusRequestError);
    connect(m_modbusBridge, &IModbusBridge::connectionLost,
            this, &StateMachine::onModbusConnectionLost);
    connect(m_modbusBridge, &IModbusBridge::connectionRestored,
            this, &StateMachine::onModbusConnectionRestored);

    // connect(m_dataProcessor, &DataProcessor::decisionReady,
    //         this, &StateMachine::onDecisionReady);

    connect(m_pollTimer, &QTimer::timeout, this, &StateMachine::pollModbus);
}

StateMachine::~StateMachine() { stop(); }

void StateMachine::start()
{
    // m_config = config;
    m_dataProcessor->onConfigChanged(m_config);
    m_dataProcessor->reset();
    m_currentRunId = static_cast<int>(QDateTime::currentSecsSinceEpoch());

    int pollMs = ConfigData().LoadConfig().pollFrequencyMs;
    m_pollTimer->start(pollMs);

    Data startEvt;
    startEvt.frame.timestampMs = QDateTime::currentMSecsSinceEpoch();
    startEvt.frame.stage = -1;
    startEvt.state = DiagState::Ok;
    m_dataStore->writeData(startEvt);

    transitionTo(DiagState::Ok);
    m_currentStageIndex = 0;
    m_modbusBridge->onReadSensors();
}

void StateMachine::stop()
{
    m_pollTimer->stop();
    m_stageTimer->stop();
    m_modbusBridge->stopPolling();

    Data stopEvt;
    stopEvt.frame.timestampMs = QDateTime::currentMSecsSinceEpoch();
    stopEvt.frame.stage = m_currentStageIndex;
    m_dataStore->writeData(stopEvt);

    transitionTo(DiagState::IDLE);
    emit finished(m_state);
}

void StateMachine::requestNextStage() { /* ... как раньше ... */ }
void StateMachine::requestAbort(const QString& reason) { /* ... */ }

// ---------- Слоты ----------
void StateMachine::pollModbus() {
    m_modbusBridge->onReadSensors();
}

void StateMachine::onSensorsDataReady(const SensorFrame& frame)
{
    m_currentSensorFrame = frame;
    m_currentSensorFrame.timestampMs = QDateTime::currentMSecsSinceEpoch();
    m_currentSensorFrame.stage = m_currentStageIndex;

    m_dataProcessor->processFrame(m_currentSensorFrame);

    // Отправка сырого кадра фронту через метод коммуникатора
    m_communicator->SendSensorFrameToFrontend(m_currentSensorFrame);
}

void StateMachine::onDecisionReady(const Decision& decision)
{
    // Запись в хранилище
    Data rec;
    rec.frame.stage = m_currentSensorFrame.stage;
    rec.frame.timestampMs = m_currentSensorFrame.timestampMs;
    rec.frame.rpm = m_currentSensorFrame.rpm;
    rec.frame.torque = m_currentSensorFrame.torque;
    rec.frame.dieselTemp = m_currentSensorFrame.dieselTemp;
    rec.frame.motorTemp = m_currentSensorFrame.motorTemp;
    rec.frame.resistorTemp = 0;                    // маппинг уточнить
    rec.frame.dieselPressure = m_currentSensorFrame.dieselPressure;
    m_dataStore->writeData(rec);

    // Обработка смены состояния
    if (decision.state != m_state) {
        Data evt;
        evt.frame.timestampMs = QDateTime::currentMSecsSinceEpoch();
        evt.frame.stage = m_currentStageIndex;
        evt.state = DiagState::Ok;
        m_dataStore->writeData(evt);

        applyControls(decision.controls);
        transitionTo(decision.state);

        // Если аварийное состояние – уведомить фронт
        if (decision.state >= DiagState::Alarm_RpmOverspeed) {
            m_communicator->SendEmergencyStopInfoToFrontend();
        }
    }

    // Отправка данных для фронта (вектор из одного элемента)
    QVector<Data> dataVec;
    Data data;
    data.frame = m_currentSensorFrame;
    data.state = decision.state;
    dataVec.append(data);
    m_communicator->SendDataToFrontend(dataVec);

    // Обратная связь: считается успешным, если состояние не аварийное
    //bool ok = (decision.state == DiagState::Ok || decision.state == DiagState::PreWarn_RpmHigh /* и т.п.*/);
    m_communicator->SendFeedbackToFrontend(decision.state);
}

void StateMachine::onReceivedFrontControl(const ModelControl& control)
{
    if (control.type == ControlType::EmergencyStop) {
        requestAbort("Front control emergency stop");
    }
    // другая логика
}

void StateMachine::onReceivedModelConfig(const ModelConfig& config)
{
    m_config = config;
    m_modbusBridge->onWriteConfig(config);
    m_dataProcessor->onConfigChanged(config);
}

void StateMachine::transitionTo(DiagState newState) {
    if (m_state == newState) return;
    m_state = newState;
}

void StateMachine::applyControls(const QVector<ModelControl>& controls) {
    // Заглушка – реализовать после расширения IModbusBridge методами записи
    for (const auto& ctrl : controls) Q_UNUSED(ctrl);
}

// ---------- Ошибки Modbus ----------
void StateMachine::onModbusConfigurationError(const QString& reason) {
    // Логируем причину в хранилище
    Data evt;
    evt.frame.timestampMs = QDateTime::currentMSecsSinceEpoch();
    evt.frame.stage = m_currentStageIndex;
    evt.state = DiagState::Alarm_Generic;
    m_dataStore->writeData(evt);
    m_communicator->SendEmergencyStopInfoToFrontend();
}

void StateMachine::onModbusConnectionError(const QString& reason) {
    // аналогично
    m_communicator->SendEmergencyStopInfoToFrontend();
}

void StateMachine::onModbusRequestError(const QString& reason) {
    m_communicator->SendEmergencyStopInfoToFrontend();
}

void StateMachine::onModbusConnectionLost() {
    m_communicator->SendEmergencyStopInfoToFrontend();
    m_pollTimer->stop();
}

void StateMachine::onModbusConnectionRestored() {
    if (m_state != DiagState::IDLE && m_state != DiagState::Alarm_Generic)
        m_pollTimer->start();
}
