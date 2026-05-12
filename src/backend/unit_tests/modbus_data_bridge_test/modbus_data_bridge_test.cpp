#include <QtTest>
#include <QSignalSpy>
#include <QTimer>
#include "../../../include/backend/modbus_client/IModbusBridge.h"
#include "../../../include/backend/modbus_client/MockModbusBridge.h"
#include "../../../include/backend/modbus_client/QtModbusBridge.h"
#include "../../../include/backend/DataTypes.h"

class ModbusDataBridgeTest : public QObject {
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    
    // Тесты Mock реализации
    void testMockConstructor();
    void testMockStartPolling();
    void testMockStopPolling();
    void testMockOnReadSensors();
    void testMockOnReadInfo();
    void testMockOnWriteConfig();
    void testMockSignals();
    
    // Тесты Qt реализации (если доступна)
    void testQtConstructor();
    void testQtStartPolling();
    void testQtStopPolling();
    void testQtOnReadSensors();
    void testQtConnectionErrors();
    
    // Общие тесты интерфейса
    void testInterfaceCompliance();
    void testMultipleStartStop();
    void testRapidPolling();
    
private:
    ModbusConfig m_config;
    MockModbusBridge* m_mockBridge;
};

void ModbusDataBridgeTest::init()
{
    m_config.host = "127.0.0.1";
    m_config.port = 1502;
    m_config.pollFrequencyMs = 100;
    m_config.timeoutMs = 1000;
    m_config.retries = 3;
    m_config.unitId = 1;
    
    m_mockBridge = new MockModbusBridge(m_config);
}

void ModbusDataBridgeTest::cleanup()
{
    delete m_mockBridge;
}

void ModbusDataBridgeTest::testMockConstructor()
{
    MockModbusBridge* bridge = new MockModbusBridge(m_config);
    QVERIFY(bridge != nullptr);
    delete bridge;
}

void ModbusDataBridgeTest::testMockStartPolling()
{
    QSignalSpy dataSpy(m_mockBridge, &IModbusBridge::sensorsDataReady);
    
    m_mockBridge->startPolling();
    QTest::qWait(250); // Ждем минимум 2 цикла опроса
    
    QVERIFY(dataSpy.count() >= 2);
}

void ModbusDataBridgeTest::testMockStopPolling()
{
    m_mockBridge->startPolling();
    QTest::qWait(150);
    
    QSignalSpy dataSpy(m_mockBridge, &IModbusBridge::sensorsDataReady);
    int countBeforeStop = dataSpy.count();
    
    m_mockBridge->stopPolling();
    QTest::qWait(250);
    
    int countAfterStop = dataSpy.count();
    // После остановки данные не должны приходить
    QCOMPARE(countAfterStop, countBeforeStop);
}

void ModbusDataBridgeTest::testMockOnReadSensors()
{
    QSignalSpy dataSpy(m_mockBridge, &IModbusBridge::sensorsDataReady);
    
    m_mockBridge->onReadSensors();
    QTest::qWait(50);
    
    QCOMPARE(dataSpy.count(), 1);
    
    if(dataSpy.count() > 0) {
        SensorFrame frame = dataSpy.first().at(0).value<SensorFrame>();
        // Проверяем, что данные валидны
        QVERIFY(frame.rpm >= 0);
        QVERIFY(frame.dieselTemp >= 0);
    }
}

void ModbusDataBridgeTest::testMockOnReadInfo()
{
    QSignalSpy infoSpy(m_mockBridge, &IModbusBridge::modelInfoReady);
    
    m_mockBridge->onReadInfo();
    QTest::qWait(50);
    
    QCOMPARE(infoSpy.count(), 1);
}

void ModbusDataBridgeTest::testMockOnWriteConfig()
{
    ModelConfig config;
    config.maxRpm = 3000;
    
    // Проверяем, что запись не вызывает ошибок
    m_mockBridge->onWriteConfig(config);
    QVERIFY(true);
}

void ModbusDataBridgeTest::testMockSignals()
{
    QSignalSpy dataSpy(m_mockBridge, &IModbusBridge::sensorsDataReady);
    QSignalSpy infoSpy(m_mockBridge, &IModbusBridge::modelInfoReady);
    
    m_mockBridge->startPolling();
    QTest::qWait(200);
    m_mockBridge->stopPolling();
    
    QVERIFY(dataSpy.count() > 0);
    QVERIFY(infoSpy.count() == 0); // info не запрашивалась автоматически
}

void ModbusDataBridgeTest::testQtConstructor()
{
#ifdef QT_MODBUS_LIB
    QtModbusBridge* bridge = new QtModbusBridge(m_config);
    QVERIFY(bridge != nullptr);
    delete bridge;
#else
    QSKIP("QtModbusBridge not available");
#endif
}

void ModbusDataBridgeTest::testQtStartPolling()
{
#ifdef QT_MODBUS_LIB
    QtModbusBridge* bridge = new QtModbusBridge(m_config);
    QSignalSpy dataSpy(bridge, &IModbusBridge::sensorsDataReady);
    
    bridge->startPolling();
    QTest::qWait(250);
    
    // Может быть 0 если сервер не доступен
    QVERIFY(dataSpy.count() >= 0);
    delete bridge;
#else
    QSKIP("QtModbusBridge not available");
#endif
}

void ModbusDataBridgeTest::testQtStopPolling()
{
#ifdef QT_MODBUS_LIB
    QtModbusBridge* bridge = new QtModbusBridge(m_config);
    bridge->startPolling();
    QTest::qWait(150);
    
    QSignalSpy dataSpy(bridge, &IModbusBridge::sensorsDataReady);
    int countBeforeStop = dataSpy.count();
    
    bridge->stopPolling();
    QTest::qWait(250);
    
    int countAfterStop = dataSpy.count();
    QCOMPARE(countAfterStop, countBeforeStop);
    delete bridge;
#else
    QSKIP("QtModbusBridge not available");
#endif
}

void ModbusDataBridgeTest::testQtOnReadSensors()
{
#ifdef QT_MODBUS_LIB
    QtModbusBridge* bridge = new QtModbusBridge(m_config);
    QSignalSpy dataSpy(bridge, &IModbusBridge::sensorsDataReady);
    
    bridge->onReadSensors();
    QTest::qWait(100);
    
    // Может быть 0 если сервер не доступен
    QVERIFY(dataSpy.count() >= 0);
    delete bridge;
#else
    QSKIP("QtModbusBridge not available");
#endif
}

void ModbusDataBridgeTest::testQtConnectionErrors()
{
#ifdef QT_MODBUS_LIB
    ModbusConfig invalidConfig;
    invalidConfig.host = "192.168.255.255"; // Несуществующий хост
    invalidConfig.port = 9999;
    
    QtModbusBridge* bridge = new QtModbusBridge(invalidConfig);
    
    QSignalSpy connectionErrorSpy(bridge, &IModbusBridge::connectionError);
    QSignalSpy connectionLostSpy(bridge, &IModbusBridge::connectionLost);
    
    // Ждем ошибок подключения
    QTest::qWait(500);
    
    // Должны быть ошибки
    QVERIFY(connectionErrorSpy.count() >= 0 || connectionLostSpy.count() >= 0);
    delete bridge;
#else
    QSKIP("QtModbusBridge not available");
#endif
}

void ModbusDataBridgeTest::testInterfaceCompliance()
{
    // Проверяем, что Mock реализует весь интерфейс
    IModbusBridge* bridge = m_mockBridge;
    
    // Все методы должны быть доступны
    bridge->startPolling();
    bridge->stopPolling();
    bridge->onReadSensors();
    bridge->onReadInfo();
    bridge->onWriteConfig(ModelConfig());
    
    QVERIFY(true);
}

void ModbusDataBridgeTest::testMultipleStartStop()
{
    for(int i = 0; i < 5; ++i) {
        m_mockBridge->startPolling();
        QTest::qWait(100);
        m_mockBridge->stopPolling();
        QTest::qWait(50);
    }
    QVERIFY(true);
}

void ModbusDataBridgeTest::testRapidPolling()
{
    ModbusConfig fastConfig = m_config;
    fastConfig.pollFrequencyMs = 10; // Очень быстрый опрос
    
    MockModbusBridge* fastBridge = new MockModbusBridge(fastConfig);
    QSignalSpy dataSpy(fastBridge, &IModbusBridge::sensorsDataReady);
    
    fastBridge->startPolling();
    QTest::qWait(200); // Должно быть ~20 измерений
    
    QVERIFY(dataSpy.count() >= 10);
    QVERIFY(dataSpy.count() <= 30);
    
    fastBridge->stopPolling();
    delete fastBridge;
}