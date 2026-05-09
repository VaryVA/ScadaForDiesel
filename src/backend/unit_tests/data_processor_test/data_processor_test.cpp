#include <QtTest>
#include <QSignalSpy>
#include "../../../include/backend/data_processing/DataProcessor.h"
#include "../../../include/backend/DataTypes.h"

class DataProcessorTest : public QObject {
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    
    // Тесты конструктора
    void testConstructor();
    
    // Тесты обработки кадров
    void testProcessFrameNormal();
    void testProcessFrameRpmOverspeed();
    void testProcessFrameDieselOverheat();
    void testProcessFrameMotorOverheat();
    void testProcessFrameResistorOverheat();
    void testProcessFramePressureOver();
    void testProcessFramePressureUnder();
    
    // Тесты предупреждений
    void testPreWarningRpmHigh();
    void testPreWarningDieselTempHigh();
    void testPreWarningMotorTempHigh();
    void testPreWarningResistorHigh();
    void testPreWarningPressureHigh();
    void testPreWarningPressureLow();
    
    // Тесты смены этапов
    void testStageChange();
    void testDifferentStages();
    
    // Тест сброса
    void testReset();
    void testAlarmLatch();
    
    // Тест конфигурации
    void testConfigChange();
    
    // Тесты сигналов
    void testDecisionReadySignal();
    
    // Тесты на граничных значениях
    void testBoundaryValues();
    void testInvalidFrames();

private:
    DataProcessor* m_processor;
    ModelConfig m_config;
};

void DataProcessorTest::init()
{
    m_config.maxRpm = 3000;
    m_config.maxDieselTemp = 120;
    m_config.maxMotorTemp = 100;
    m_config.maxResistorBalance = 150;
    m_config.maxDieselPressure = 10.0;
    m_config.minDieselPressure = 2.0;
    
    m_processor = new DataProcessor(m_config);
}

void DataProcessorTest::cleanup()
{
    delete m_processor;
}

void DataProcessorTest::testConstructor()
{
    DataProcessor* processor = new DataProcessor(m_config);
    QVERIFY(processor != nullptr);
    delete processor;
}

void DataProcessorTest::testProcessFrameNormal()
{
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    
    SensorFrame frame;
    frame.rpm = 2000;
    frame.dieselTemp = 90;
    frame.motorTemp = 80;
    frame.resistorBalance = 100;
    frame.dieselPressure = 5.0;
    frame.stage = 3; // HOT_NO_LOAD
    
    m_processor->onStageChanged(3);
    m_processor->processFrame(frame);
    
    QCOMPARE(spy.count(), 1);
    
    Decision decision = spy.takeFirst().at(0).value<Decision>();
    QCOMPARE(decision.state, DiagState::Ok);
}

void DataProcessorTest::testProcessFrameRpmOverspeed()
{
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    
    SensorFrame frame;
    frame.rpm = 3500; // Превышение (max 3000)
    frame.dieselTemp = 90;
    frame.motorTemp = 80;
    frame.resistorBalance = 100;
    frame.dieselPressure = 5.0;
    frame.stage = 3;
    
    m_processor->onStageChanged(3);
    m_processor->processFrame(frame);
    
    QCOMPARE(spy.count(), 1);
    
    Decision decision = spy.takeFirst().at(0).value<Decision>();
    QCOMPARE(decision.state, DiagState::Alarm_RpmOverspeed);
    QVERIFY(!decision.reason.isEmpty());
    QVERIFY(decision.controls.size() > 0);
    
    // Проверяем, что есть EmergencyStop
    bool hasEmergencyStop = false;
    for(const auto& control : decision.controls) {
        if(control.type == ControlType::EmergencyStop) {
            hasEmergencyStop = true;
            break;
        }
    }
    QVERIFY(hasEmergencyStop);
}

void DataProcessorTest::testProcessFrameDieselOverheat()
{
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    
    SensorFrame frame;
    frame.rpm = 2000;
    frame.dieselTemp = 150; // Превышение (max 120)
    frame.motorTemp = 80;
    frame.resistorBalance = 100;
    frame.dieselPressure = 5.0;
    frame.stage = 3;
    
    m_processor->onStageChanged(3);
    m_processor->processFrame(frame);
    
    QCOMPARE(spy.count(), 1);
    
    Decision decision = spy.takeFirst().at(0).value<Decision>();
    QCOMPARE(decision.state, DiagState::Alarm_DieselOverheat);
}

void DataProcessorTest::testProcessFrameMotorOverheat()
{
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    
    SensorFrame frame;
    frame.rpm = 2000;
    frame.dieselTemp = 90;
    frame.motorTemp = 120; // Превышение (max 100)
    frame.resistorBalance = 100;
    frame.dieselPressure = 5.0;
    
    m_processor->processFrame(frame);
    
    QCOMPARE(spy.count(), 1);
    
    Decision decision = spy.takeFirst().at(0).value<Decision>();
    QCOMPARE(decision.state, DiagState::Alarm_MotorOverheat);
}

void DataProcessorTest::testProcessFrameResistorOverheat()
{
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    
    SensorFrame frame;
    frame.rpm = 2000;
    frame.dieselTemp = 90;
    frame.motorTemp = 80;
    frame.resistorBalance = 180; // Превышение (max 150)
    frame.dieselPressure = 5.0;
    frame.stage = 4; // HOT_WITH_LOAD
    
    m_processor->onStageChanged(4);
    m_processor->processFrame(frame);
    
    QCOMPARE(spy.count(), 1);
    
    Decision decision = spy.takeFirst().at(0).value<Decision>();
    QCOMPARE(decision.state, DiagState::Alarm_ResistorOverheat);
}

void DataProcessorTest::testProcessFramePressureOver()
{
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    
    SensorFrame frame;
    frame.rpm = 2000;
    frame.dieselTemp = 90;
    frame.motorTemp = 80;
    frame.resistorBalance = 100;
    frame.dieselPressure = 12.0; // Превышение (max 10.0)
    frame.stage = 2; // START_AND_WARMUP
    
    m_processor->onStageChanged(2);
    m_processor->processFrame(frame);
    
    QCOMPARE(spy.count(), 1);
    
    Decision decision = spy.takeFirst().at(0).value<Decision>();
    QCOMPARE(decision.state, DiagState::Alarm_PressureOver);
}

void DataProcessorTest::testProcessFramePressureUnder()
{
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    
    SensorFrame frame;
    frame.rpm = 2000;
    frame.dieselTemp = 90;
    frame.motorTemp = 80;
    frame.resistorBalance = 100;
    frame.dieselPressure = 1.0; // Ниже минимума (min 2.0)
    frame.stage = 2;
    
    m_processor->onStageChanged(2);
    m_processor->processFrame(frame);
    
    QCOMPARE(spy.count(), 1);
    
    Decision decision = spy.takeFirst().at(0).value<Decision>();
    QCOMPARE(decision.state, DiagState::Alarm_PressureUnder);
}

void DataProcessorTest::testPreWarningRpmHigh()
{
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    
    SensorFrame frame;
    frame.rpm = 2750; // 90% от 3000
    frame.stage = 3;
    
    m_processor->onStageChanged(3);
    m_processor->processFrame(frame);
    
    QCOMPARE(spy.count(), 1);
    
    Decision decision = spy.takeFirst().at(0).value<Decision>();
    QCOMPARE(decision.state, DiagState::PreWarn_RpmHigh);
}

void DataProcessorTest::testPreWarningDieselTempHigh()
{
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    
    SensorFrame frame;
    frame.dieselTemp = 108; // 90% от 120
    frame.stage = 3;
    
    m_processor->processFrame(frame);
    
    QCOMPARE(spy.count(), 1);
    
    Decision decision = spy.takeFirst().at(0).value<Decision>();
    QCOMPARE(decision.state, DiagState::PreWarn_DieselTempHigh);
}

void DataProcessorTest::testPreWarningMotorTempHigh()
{
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    
    SensorFrame frame;
    frame.motorTemp = 90; // 90% от 100
    frame.stage = 3;
    
    m_processor->processFrame(frame);
    
    QCOMPARE(spy.count(), 1);
    
    Decision decision = spy.takeFirst().at(0).value<Decision>();
    QCOMPARE(decision.state, DiagState::PreWarn_MotorTempHigh);
}

void DataProcessorTest::testStageChange()
{
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    
    SensorFrame frame;
    frame.rpm = 3500; // Вызовет аварию
    
    m_processor->onStageChanged(1); // COLD_CRANKING
    m_processor->processFrame(frame);
    
    QCOMPARE(spy.count(), 1);
    
    Decision decision = spy.takeFirst().at(0).value<Decision>();
    QCOMPARE(decision.state, DiagState::Alarm_RpmOverspeed);
}

void DataProcessorTest::testDifferentStages()
{
    QVector<int> stages = {1, 2, 3, 4, 5, 6};
    SensorFrame normalFrame;
    normalFrame.rpm = 2000;
    normalFrame.dieselTemp = 90;
    normalFrame.motorTemp = 80;
    
    for(int stage : stages) {
        QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
        m_processor->onStageChanged(stage);
        m_processor->processFrame(normalFrame);
        
        // В неактивных этапах не должно быть проверок
        if(stage == 0 || stage == 5 || stage == 6) {
            QCOMPARE(spy.count(), 1);
            Decision decision = spy.takeFirst().at(0).value<Decision>();
            QCOMPARE(decision.state, DiagState::Ok);
        }
    }
}

void DataProcessorTest::testReset()
{
    SensorFrame alarmFrame;
    alarmFrame.rpm = 3500;
    alarmFrame.stage = 3;
    
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    
    m_processor->onStageChanged(3);
    m_processor->processFrame(alarmFrame);
    QCOMPARE(spy.count(), 1);
    
    // После аварии новые фреймы не должны обрабатываться
    m_processor->processFrame(alarmFrame);
    QCOMPARE(spy.count(), 1); // Количество не изменилось
    
    m_processor->reset();
    
    // После сброса должно снова обрабатывать
    m_processor->processFrame(alarmFrame);
    QCOMPARE(spy.count(), 2); // Увеличилось
}

void DataProcessorTest::testAlarmLatch()
{
    SensorFrame alarmFrame;
    alarmFrame.rpm = 3500;
    alarmFrame.stage = 3;
    
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    
    m_processor->onStageChanged(3);
    m_processor->processFrame(alarmFrame);
    QCOMPARE(spy.count(), 1);
    
    SensorFrame normalFrame;
    normalFrame.rpm = 2000;
    normalFrame.stage = 3;
    
    m_processor->processFrame(normalFrame);
    QCOMPARE(spy.count(), 1); // Не увеличилось - защелка аварии
}

void DataProcessorTest::testConfigChange()
{
    ModelConfig newConfig;
    newConfig.maxRpm = 4000;
    newConfig.maxDieselTemp = 150;
    
    m_processor->onConfigChanged(newConfig);
    
    SensorFrame frame;
    frame.rpm = 3800; // Было бы аварией со старой конфигурацией
    frame.stage = 3;
    
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    m_processor->onStageChanged(3);
    m_processor->processFrame(frame);
    
    QCOMPARE(spy.count(), 1);
    Decision decision = spy.takeFirst().at(0).value<Decision>();
    QCOMPARE(decision.state, DiagState::Ok); // С новой конфигурацией - норма
}

void DataProcessorTest::testDecisionReadySignal()
{
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    
    SensorFrame frame;
    frame.rpm = 2000;
    frame.stage = 3;
    
    m_processor->onStageChanged(3);
    m_processor->processFrame(frame);
    
    QCOMPARE(spy.count(), 1);
    QVERIFY(spy.isValid());
}

void DataProcessorTest::testBoundaryValues()
{
    QSignalSpy spy(m_processor, &DataProcessor::decisionReady);
    
    // Точные граничные значения
    SensorFrame frame;
    frame.rpm = 3000; // Точно на границе
    frame.dieselTemp = 120; // Точно на границе
    frame.motorTemp = 100; // Точно на границе
    frame.resistorBalance = 150; // Точно на границе
    frame.dieselPressure = 10.0; // Точно на границе
    frame.stage = 4;
    
    m_processor->onStageChanged(4);
    m_processor->processFrame(frame);
    
    QCOMPARE(spy.count(), 1);
    Decision decision = spy.takeFirst().at(0).value<Decision>();
    QCOMPARE(decision.state, DiagState::Alarm_RpmOverspeed); // Должна быть авария
}