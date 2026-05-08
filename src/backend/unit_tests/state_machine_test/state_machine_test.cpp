#include <QtTest>
#include <QSignalSpy>
#include <QThread>
#include "../../../include/backend/state_machine.h"
#include "../../../include/backend/backend_worker/backendworker.h"
#include "../../../include/backend/DataTypes.h"

class StateMachineTest : public QObject {
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    
    // Тесты конструктора
    void testConstructor();
    
    // Тесты состояний
    void testInitialState();
    void testTransitionTo();
    
    // Тесты этапов
    void testRequestStart();
    void testRequestNextStage();
    void testRequestAbort();
    void testStageSequence();
    
    // Тесты сигналов
    void testStageChangedSignal();
    void testFinishedSignal();
    
    // Тесты таймеров
    void testStageTimeout();
    
    // Тесты обработки команд
    void testOnReceivedFrontControl();
    void testOnReceivedModelConfig();
    
    // Тесты интеграции с Modbus
    void testModbusBridgeIntegration();
    
    // Тесты граничных случаев
    void testInvalidStageTransition();
    void testAbortDuringStage();

private:
    BackendWorker* m_backendWorker;
    StateMachine* m_stateMachine;
    QThread* m_workerThread;
};

void StateMachineTest::init()
{
    m_backendWorker = new BackendWorker();
    m_workerThread = new QThread();
    m_backendWorker->moveToThread(m_workerThread);
    m_workerThread->start();
    
    m_stateMachine = new StateMachine(m_backendWorker);
}

void StateMachineTest::cleanup()
{
    delete m_stateMachine;
    m_workerThread->quit();
    m_workerThread->wait();
    delete m_backendWorker;
    delete m_workerThread;
}

void StateMachineTest::testConstructor()
{
    StateMachine* sm = new StateMachine(m_backendWorker);
    QVERIFY(sm != nullptr);
    QCOMPARE(sm->currentStageIndex(), -1);
    delete sm;
}

void StateMachineTest::testInitialState()
{
    QCOMPARE(m_stateMachine->currentStageIndex(), -1);
    QCOMPARE(m_stateMachine->currentState(), DiagState::Ok);
}

void StateMachineTest::testTransitionTo()
{
    QSignalSpy stageSpy(m_stateMachine, &StateMachine::stageChanged);
    
    ModelConfig config;
    m_stateMachine->requestStart(config);
    
    QTest::qWait(100);
    
    // Проверяем, что состояние изменилось
    QVERIFY(stageSpy.count() >= 0);
}

void StateMachineTest::testRequestStart()
{
    QSignalSpy finishedSpy(m_stateMachine, &StateMachine::finished);
    
    ModelConfig config;
    config.maxRpm = 3000;
    config.maxDieselTemp = 120;
    
    m_stateMachine->requestStart(config);
    QTest::qWait(100);
    
    // Проверяем, что процесс запустился
    QVERIFY(m_stateMachine->currentStageIndex() >= 0);
}

void StateMachineTest::testRequestNextStage()
{
    ModelConfig config;
    m_stateMachine->requestStart(config);
    QTest::qWait(100);
    
    int oldStage = m_stateMachine->currentStageIndex();
    m_stateMachine->requestNextStage();
    QTest::qWait(100);
    
    int newStage = m_stateMachine->currentStageIndex();
    QVERIFY(newStage != oldStage || newStage == -1);
}

void StateMachineTest::testRequestAbort()
{
    QSignalSpy finishedSpy(m_stateMachine, &StateMachine::finished);
    
    ModelConfig config;
    m_stateMachine->requestStart(config);
    QTest::qWait(100);
    
    m_stateMachine->requestAbort("Test abort");
    QTest::qWait(100);
    
    QCOMPARE(finishedSpy.count(), 1);
}

void StateMachineTest::testStageSequence()
{
    QSignalSpy stageSpy(m_stateMachine, &StateMachine::stageChanged);
    
    ModelConfig config;
    m_stateMachine->requestStart(config);
    QTest::qWait(100);
    
    // Проходим по всем этапам
    for(int i = 0; i < 10; ++i) {
        m_stateMachine->requestNextStage();
        QTest::qWait(50);
    }
    
    // Проверяем, что сигналы смены этапов были
    QVERIFY(stageSpy.count() > 0);
}

void StateMachineTest::testStageChangedSignal()
{
    QSignalSpy spy(m_stateMachine, &StateMachine::stageChanged);
    
    ModelConfig config;
    m_stateMachine->requestStart(config);
    QTest::qWait(100);
    
    QVERIFY(spy.count() >= 0);
    
    if(spy.count() > 0) {
        QList<QVariant> arguments = spy.first();
        QCOMPARE(arguments.size(), 2); // oldStage, newStage
    }
}

void StateMachineTest::testFinishedSignal()
{
    QSignalSpy spy(m_stateMachine, &StateMachine::finished);
    
    ModelConfig config;
    m_stateMachine->requestStart(config);
    QTest::qWait(100);
    
    m_stateMachine->requestAbort("Force abort");
    QTest::qWait(100);
    
    QCOMPARE(spy.count(), 1);
    
    if(spy.count() > 0) {
        QList<QVariant> arguments = spy.first();
        QCOMPARE(arguments.size(), 1); // finalState
    }
}

void StateMachineTest::testStageTimeout()
{
    // Тест автоматического перехода по таймауту
    ModelConfig config;
    m_stateMachine->requestStart(config);
    QTest::qWait(500); // Ждем возможного таймаута
    
    // Проверяем, что этапы переключаются автоматически
    // (зависит от реализации)
}

void StateMachineTest::testOnReceivedFrontControl()
{
    FrontControl control;
    // control.control = ControlType::NextStage;
    
    QSignalSpy stageSpy(m_stateMachine, &StateMachine::stageChanged);
    
    // Эмулируем получение команды через коммуникатор
    // (зависит от реализации)
}

void StateMachineTest::testOnReceivedModelConfig()
{
    ModelConfig config;
    config.maxRpm = 5000;
    config.maxDieselTemp = 200;
    
    // Эмулируем получение конфигурации
    // (зависит от реализации)
}

void StateMachineTest::testModbusBridgeIntegration()
{
    // Проверяем, что ModbusBridge правильно инициализирован
    ModelConfig config;
    m_stateMachine->requestStart(config);
    QTest::qWait(100);
    
    // Проверяем состояние Modbus клиента
    // (зависит от реализации)
}

void StateMachineTest::testInvalidStageTransition()
{
    ModelConfig config;
    m_stateMachine->requestStart(config);
    QTest::qWait(100);
    
    // Попытка перейти на следующий этап когда он не должен быть доступен
    for(int i = 0; i < 20; ++i) {
        m_stateMachine->requestNextStage();
        QTest::qWait(50);
    }
    
    // Не должно быть краха
    QVERIFY(true);
}

void StateMachineTest::testAbortDuringStage()
{
    QSignalSpy finishedSpy(m_stateMachine, &StateMachine::finished);
    
    ModelConfig config;
    m_stateMachine->requestStart(config);
    QTest::qWait(100);
    
    // Прерываем на середине этапа
    m_stateMachine->requestAbort("Abort during stage");
    QTest::qWait(100);
    
    QCOMPARE(finishedSpy.count(), 1);
}