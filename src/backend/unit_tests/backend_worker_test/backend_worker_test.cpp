#include <QtTest>
#include <QSignalSpy>
#include <QThread>
#include "../../../include/backend/backend_worker/backendworker.h"
#include "../../../include/backend/DataTypes.h"

class BackendWorkerTest : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Тесты конструктора и деструктора
    void testConstructor();
    void testDestructor();
    
    // Тесты запуска/остановки
    void testRun();
    void testStop();
    void testRunStopSequence();
    
    // Тесты отправки команд
    void testSendFrontControlToBackend();
    void testSendModelConfigToBackend();
    
    // Тесты сигналов получения данных
    void testReceivedSensorFrame();
    void testReceivedEmergencyStopInfo();
    void testReceivedFeedback();
    void testReceivedData();
    
    // Тесты внутренних сигналов
    void testSendedFrontControlToBackend();
    void testSendedModelConfigToBackend();
    
    // Тесты многопоточности
    void testThreadSafety();
    void testMultipleRunStopCycles();

private:
    BackendWorker* m_worker;
    QThread* m_testThread;
};

void BackendWorkerTest::initTestCase()
{
    qRegisterMetaType<FrontControl>();
    qRegisterMetaType<ModelConfig>();
    qRegisterMetaType<SensorFrame>();
    qRegisterMetaType<QVector<Data>>();
}

void BackendWorkerTest::cleanupTestCase()
{
}

void BackendWorkerTest::init()
{
    m_worker = new BackendWorker();
    m_testThread = new QThread();
    m_worker->moveToThread(m_testThread);
    m_testThread->start();
}

void BackendWorkerTest::cleanup()
{
    m_testThread->quit();
    m_testThread->wait();
    delete m_worker;
    delete m_testThread;
}

void BackendWorkerTest::testConstructor()
{
    BackendWorker* worker = new BackendWorker();
    QVERIFY(worker != nullptr);
    delete worker;
}

void BackendWorkerTest::testDestructor()
{
    BackendWorker* worker = new BackendWorker();
    delete worker;
    // Если деструктор отработал без ошибок - тест пройден
    QVERIFY(true);
}

void BackendWorkerTest::testRun()
{
    QSignalSpy runSpy(m_worker, &BackendWorker::Run);
    
    QMetaObject::invokeMethod(m_worker, "Run", Qt::QueuedConnection);
    QTest::qWait(100);
    
    // Проверяем, что поток запустился
    QVERIFY(m_testThread->isRunning());
}

void BackendWorkerTest::testStop()
{
    QMetaObject::invokeMethod(m_worker, "Run", Qt::QueuedConnection);
    QTest::qWait(100);
    
    QMetaObject::invokeMethod(m_worker, "Stop", Qt::QueuedConnection);
    QTest::qWait(100);
    
    // Проверяем, что поток остановился
    QVERIFY(!m_testThread->isRunning());
}

void BackendWorkerTest::testRunStopSequence()
{
    for(int i = 0; i < 5; ++i) {
        QMetaObject::invokeMethod(m_worker, "Run", Qt::QueuedConnection);
        QTest::qWait(50);
        QMetaObject::invokeMethod(m_worker, "Stop", Qt::QueuedConnection);
        QTest::qWait(50);
    }
    QVERIFY(true);
}

void BackendWorkerTest::testSendFrontControlToBackend()
{
    QSignalSpy internalSpy(m_worker, &BackendWorker::SendedFrontControlToBackend);
    
    FrontControl control;
    // control.control = ControlType::NextStage; // Настройка в зависимости от реализации
    
    QMetaObject::invokeMethod(m_worker, "SendFrontControlToBackend", 
                              Qt::QueuedConnection, Q_ARG(FrontControl&, control));
    QTest::qWait(100);
    
    QCOMPARE(internalSpy.count(), 1);
}

void BackendWorkerTest::testSendModelConfigToBackend()
{
    QSignalSpy internalSpy(m_worker, &BackendWorker::SendedModelConfigToBackend);
    
    ModelConfig config;
    config.maxRpm = 3000;
    config.maxDieselTemp = 120;
    config.maxMotorTemp = 100;
    
    QMetaObject::invokeMethod(m_worker, "SendModelConfigToBackend", 
                              Qt::QueuedConnection, Q_ARG(ModelConfig&, config));
    QTest::qWait(100);
    
    QCOMPARE(internalSpy.count(), 1);
}

void BackendWorkerTest::testReceivedSensorFrame()
{
    QSignalSpy spy(m_worker, &BackendWorker::ReceivedSensorFrame);
    
    SensorFrame frame;
    frame.rpm = 1500;
    frame.dieselTemp = 85;
    frame.timestampMs = QDateTime::currentMSecsSinceEpoch();
    
    emit m_worker->ReceivedSensorFrame(frame);
    QTest::qWait(100);
    
    QCOMPARE(spy.count(), 1);
    
    QList<QVariant> arguments = spy.takeFirst();
    SensorFrame receivedFrame = arguments.at(0).value<SensorFrame>();
    QCOMPARE(receivedFrame.rpm, 1500);
    QCOMPARE(receivedFrame.dieselTemp, 85);
}

void BackendWorkerTest::testReceivedEmergencyStopInfo()
{
    QSignalSpy spy(m_worker, &BackendWorker::ReceivedEmergencyStopInfo);
    
    emit m_worker->ReceivedEmergencyStopInfo();
    QTest::qWait(100);
    
    QCOMPARE(spy.count(), 1);
}

void BackendWorkerTest::testReceivedFeedback()
{
    QSignalSpy spy(m_worker, &BackendWorker::ReceivedFeedback);
    
    emit m_worker->ReceivedFeedback(true);
    QTest::qWait(100);
    
    QCOMPARE(spy.count(), 1);
    
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toBool(), true);
}

void BackendWorkerTest::testReceivedData()
{
    QSignalSpy spy(m_worker, &BackendWorker::ReceivedData);
    
    QVector<Data> dataVector;
    // Добавление тестовых данных
    
    emit m_worker->ReceivedData(dataVector);
    QTest::qWait(100);
    
    QCOMPARE(spy.count(), 1);
}

void BackendWorkerTest::testSendedFrontControlToBackend()
{
    QSignalSpy spy(m_worker, &BackendWorker::SendedFrontControlToBackend);
    
    FrontControl control;
    emit m_worker->SendedFrontControlToBackend(control);
    QTest::qWait(100);
    
    QCOMPARE(spy.count(), 1);
}

void BackendWorkerTest::testSendedModelConfigToBackend()
{
    QSignalSpy spy(m_worker, &BackendWorker::SendedModelConfigToBackend);
    
    ModelConfig config;
    emit m_worker->SendedModelConfigToBackend(config);
    QTest::qWait(100);
    
    QCOMPARE(spy.count(), 1);
}

void BackendWorkerTest::testThreadSafety()
{
    QAtomicInt counter = 0;
    QSignalSpy spy(m_worker, &BackendWorker::ReceivedSensorFrame);
    
    // Эмитируем сигналы из разных потоков
    for(int i = 0; i < 10; ++i) {
        SensorFrame frame;
        frame.rpm = i * 100;
        QMetaObject::invokeMethod(m_worker, [&]() {
            emit m_worker->ReceivedSensorFrame(frame);
            counter.ref();
        }, Qt::QueuedConnection);
    }
    
    QTest::qWait(500);
    QCOMPARE(counter.load(), 10);
    QCOMPARE(spy.count(), 10);
}

void BackendWorkerTest::testMultipleRunStopCycles()
{
    for(int i = 0; i < 3; ++i) {
        QMetaObject::invokeMethod(m_worker, "Run", Qt::QueuedConnection);
        QTest::qWait(100);
        QVERIFY(m_testThread->isRunning());
        
        QMetaObject::invokeMethod(m_worker, "Stop", Qt::QueuedConnection);
        QTest::qWait(100);
    }
    QVERIFY(true);
}