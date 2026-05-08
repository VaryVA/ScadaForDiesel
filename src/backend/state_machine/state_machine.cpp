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
    m_measurementConnector = new CSVConnector("measurements.csv");
    m_eventConnector       = new CSVConnector("events.csv");

    // ----- Создаём DataStore -----
    m_dataStore = new DataStore(m_measurementConnector, m_eventConnector, this);

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

    EventRecord startEvt;
    startEvt.runId = m_currentRunId;
    startEvt.timestampMs = QDateTime::currentMSecsSinceEpoch();
    startEvt.stage = -1;
    startEvt.type = "start";
    startEvt.message = "Experiment started";
    m_dataStore->writeEvent(startEvt);

    transitionTo(DiagState::Ok);
    m_currentStageIndex = 0;
    m_modbusBridge->onReadSensors();
}

void StateMachine::stop()
{
    m_pollTimer->stop();
    m_stageTimer->stop();
    m_modbusBridge->stopPolling();

    EventRecord stopEvt;
    stopEvt.runId = m_currentRunId;
    stopEvt.timestampMs = QDateTime::currentMSecsSinceEpoch();
    stopEvt.stage = m_currentStageIndex;
    stopEvt.type = "stop";
    stopEvt.message = "Experiment stopped";
    m_dataStore->writeEvent(stopEvt);

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
    MeasurementRecord rec;
    rec.runId = m_currentRunId;
    rec.stage = m_currentSensorFrame.stage;
    rec.timestampMs = m_currentSensorFrame.timestampMs;
    rec.rpm = m_currentSensorFrame.rpm;
    rec.torque = m_currentSensorFrame.torque;
    rec.dieselTemp = m_currentSensorFrame.dieselTemp;
    rec.motorTemp = m_currentSensorFrame.motorTemp;
    rec.resistorTemp = 0;                    // маппинг уточнить
    rec.dieselPressure = m_currentSensorFrame.dieselPressure;
    rec.throttle = 0;                        // дополнить при необходимости
    rec.brakeTorque = 0;
    rec.flags = (decision.state == DiagState::Ok) ? "OK" : "WARNING";
    m_dataStore->writeRecord(rec);

    // Обработка смены состояния
    if (decision.state != m_state) {
        EventRecord evt;
        evt.runId = m_currentRunId;
        evt.timestampMs = QDateTime::currentMSecsSinceEpoch();
        evt.stage = m_currentStageIndex;
        evt.type = "transition";
        evt.message = QString("State changed to %1").arg(static_cast<int>(decision.state));
        m_dataStore->writeEvent(evt);

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
    bool ok = (decision.state == DiagState::Ok || decision.state == DiagState::PreWarn_RpmHigh /* и т.п.*/);
    m_communicator->SendFeedbackToFrontend(ok);
}

void StateMachine::onReceivedFrontControl(const FrontControl& control)
{
    if (control.state == DiagState::Alarm_Generic) {
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
    EventRecord evt;
    evt.runId = m_currentRunId;
    evt.timestampMs = QDateTime::currentMSecsSinceEpoch();
    evt.stage = m_currentStageIndex;
    evt.type = "error";
    evt.message = "Modbus config error: " + reason;
    m_dataStore->writeEvent(evt);
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
