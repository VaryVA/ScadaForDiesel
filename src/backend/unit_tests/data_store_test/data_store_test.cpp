#include <QtTest>
#include <QFile>
#include <QDir>
#include <QTemporaryDir>
#include "../../../include/backend/data_store/DataStore.h"

class DataStoreTest : public QObject {
    Q_OBJECT
private slots:
    void init();
    void cleanup();
    
    // Тесты конструктора
    void testConstructor();
    
    // Тесты записи
    void testWriteRecord();
    void testWriteEvent();
    void testMultipleWriteRecord();
    void testMultipleWriteEvent();
    
    // Тесты чтения
    void testReadRecords();
    void testReadEvents();
    void testReadNonExistent();
    
    // Тесты отчета
    void testBuildReportCompleted();
    void testBuildReportAborted();
    
    // Тесты работы с файлами
    void testFileCreation();
    void testHeadersWritten();
    void testEmptyFileRead();
    
    // Тесты граничных случаев
    void testLargeData();
    void testConcurrentWrites();

private:
    QTemporaryDir m_tempDir;
    CSVConnector* m_measurementConnector;
    CSVConnector* m_eventConnector;
    DataStore* m_dataStore;
    QString m_measurementPath;
    QString m_eventPath;
};

void DataStoreTest::init()
{
    QVERIFY(m_tempDir.isValid());
    m_measurementPath = m_tempDir.path() + "/measurements.csv";
    m_eventPath = m_tempDir.path() + "/events.csv";
    
    m_measurementConnector = new CSVConnector(m_measurementPath.toStdString());
    m_eventConnector = new CSVConnector(m_eventPath.toStdString());
    m_dataStore = new DataStore(m_measurementConnector, m_eventConnector);
}

void DataStoreTest::cleanup()
{
    delete m_dataStore;
    delete m_measurementConnector;
    delete m_eventConnector;
    
    // Очищаем временные файлы
    QFile::remove(m_measurementPath);
    QFile::remove(m_eventPath);
}

void DataStoreTest::testConstructor()
{
    DataStore* store = new DataStore(m_measurementConnector, m_eventConnector);
    QVERIFY(store != nullptr);
    delete store;
}

void DataStoreTest::testWriteRecord()
{
    MeasurementRecord record;
    record.runId = 1;
    record.stage = 1;
    record.timestampMs = QDateTime::currentMSecsSinceEpoch();
    record.rpm = 1500;
    record.torque = 100;
    record.dieselTemp = 85;
    record.motorTemp = 75;
    record.resistorTemp = 60;
    record.dieselPressure = 5.0;
    record.throttle = 50;
    record.brakeTorque = 30;
    record.flags = "OK";
    
    m_dataStore->writeRecord(record);
    QTest::qWait(100);
    
    QVector<MeasurementRecord> records = m_dataStore->readRecords(1);
    QCOMPARE(records.size(), 1);
    QCOMPARE(records[0].runId, record.runId);
    QCOMPARE(records[0].rpm, record.rpm);
}

void DataStoreTest::testWriteEvent()
{
    EventRecord event;
    event.runId = 1;
    event.timestampMs = QDateTime::currentMSecsSinceEpoch();
    event.stage = 1;
    event.type = "stage_change";
    event.message = "Entered stage 1";
    
    m_dataStore->writeEvent(event);
    QTest::qWait(100);
    
    QVector<EventRecord> events = m_dataStore->readEvents(1);
    QCOMPARE(events.size(), 1);
    QCOMPARE(events[0].type, event.type);
    QCOMPARE(events[0].message, event.message);
}

void DataStoreTest::testMultipleWriteRecord()
{
    const int recordCount = 10;
    
    for(int i = 0; i < recordCount; ++i) {
        MeasurementRecord record;
        record.runId = 1;
        record.stage = i;
        record.timestampMs = QDateTime::currentMSecsSinceEpoch();
        record.rpm = i * 100;
        m_dataStore->writeRecord(record);
    }
    
    QTest::qWait(200);
    
    QVector<MeasurementRecord> records = m_dataStore->readRecords(1);
    QCOMPARE(records.size(), recordCount);
    
    for(int i = 0; i < recordCount; ++i) {
        QCOMPARE(records[i].stage, i);
        QCOMPARE(records[i].rpm, i * 100);
    }
}

void DataStoreTest::testMultipleWriteEvent()
{
    const int eventCount = 10;
    
    for(int i = 0; i < eventCount; ++i) {
        EventRecord event;
        event.runId = 1;
        event.timestampMs = QDateTime::currentMSecsSinceEpoch();
        event.stage = i;
        event.type = "test";
        event.message = QString("Event %1").arg(i);
        m_dataStore->writeEvent(event);
    }
    
    QTest::qWait(200);
    
    QVector<EventRecord> events = m_dataStore->readEvents(1);
    QCOMPARE(events.size(), eventCount);
}

void DataStoreTest::testReadRecords()
{
    // Записываем данные
    MeasurementRecord record;
    record.runId = 42;
    record.rpm = 2000;
    m_dataStore->writeRecord(record);
    
    QTest::qWait(100);
    
    // Читаем данные
    QVector<MeasurementRecord> records = m_dataStore->readRecords(42);
    QCOMPARE(records.size(), 1);
    QCOMPARE(records[0].runId, 42);
    QCOMPARE(records[0].rpm, 2000);
}

void DataStoreTest::testReadEvents()
{
    EventRecord event;
    event.runId = 42;
    event.type = "test_event";
    m_dataStore->writeEvent(event);
    
    QTest::qWait(100);
    
    QVector<EventRecord> events = m_dataStore->readEvents(42);
    QCOMPARE(events.size(), 1);
    QCOMPARE(events[0].type, "test_event");
}

void DataStoreTest::testReadNonExistent()
{
    QVector<MeasurementRecord> records = m_dataStore->readRecords(999);
    QVERIFY(records.isEmpty());
    
    QVector<EventRecord> events = m_dataStore->readEvents(999);
    QVERIFY(events.isEmpty());
}

void DataStoreTest::testBuildReportCompleted()
{
    // Записываем тестовые данные для завершенного испытания
    for(int i = 0; i < 5; ++i) {
        MeasurementRecord record;
        record.runId = 100;
        record.stage = i;
        record.timestampMs = QDateTime::currentMSecsSinceEpoch();
        m_dataStore->writeRecord(record);
        
        EventRecord event;
        event.runId = 100;
        event.stage = i;
        event.type = "stage_change";
        event.message = QString("Stage %1 completed").arg(i);
        m_dataStore->writeEvent(event);
    }
    
    EventRecord finalEvent;
    finalEvent.runId = 100;
    finalEvent.type = "completed";
    finalEvent.message = "Test completed successfully";
    m_dataStore->writeEvent(finalEvent);
    
    QTest::qWait(200);
    
    Report report = m_dataStore->buildReport(100);
    QCOMPARE(report.runId, 100);
    QCOMPARE(report.finalStatus, "completed");
    QVERIFY(report.stages.size() > 0);
    QVERIFY(report.events.size() > 0);
}

void DataStoreTest::testBuildReportAborted()
{
    EventRecord abortEvent;
    abortEvent.runId = 101;
    abortEvent.type = "abort";
    abortEvent.message = "Test aborted due to alarm";
    m_dataStore->writeEvent(abortEvent);
    
    QTest::qWait(100);
    
    Report report = m_dataStore->buildReport(101);
    QCOMPARE(report.runId, 101);
    QCOMPARE(report.finalStatus, "aborted");
}

void DataStoreTest::testFileCreation()
{
    // Проверяем, что файлы создаются
    QVERIFY(QFile::exists(m_measurementPath));
    QVERIFY(QFile::exists(m_eventPath));
    
    // Проверяем, что файлы не пустые (есть заголовки)
    QFile measFile(m_measurementPath);
    measFile.open(QIODevice::ReadOnly);
    QVERIFY(measFile.size() > 0);
    measFile.close();
}

void DataStoreTest::testHeadersWritten()
{
    // Проверяем наличие заголовков в CSV
    QFile measFile(m_measurementPath);
    measFile.open(QIODevice::ReadOnly);
    QString firstLine = measFile.readLine();
    measFile.close();
    
    QVERIFY(firstLine.contains("runId") || firstLine.contains("timestamp"));
}

void DataStoreTest::testEmptyFileRead()
{
    QVector<MeasurementRecord> records = m_dataStore->readRecords(999);
    QVERIFY(records.isEmpty());
}

void DataStoreTest::testLargeData()
{
    const int largeCount = 1000;
    
    for(int i = 0; i < largeCount; ++i) {
        MeasurementRecord record;
        record.runId = 200;
        record.stage = i % 5;
        record.rpm = i;
        m_dataStore->writeRecord(record);
    }
    
    QTest::qWait(1000);
    
    QVector<MeasurementRecord> records = m_dataStore->readRecords(200);
    QCOMPARE(records.size(), largeCount);
    
    // Проверяем, что данные корректны
    for(int i = 0; i < largeCount; ++i) {
        QCOMPARE(records[i].rpm, i);
    }
}

void DataStoreTest::testConcurrentWrites()
{
    // Тест одновременной записи разных runId
    for(int runId = 0; runId < 10; ++runId) {
        MeasurementRecord record;
        record.runId = runId;
        record.rpm = runId * 100;
        m_dataStore->writeRecord(record);
    }
    
    QTest::qWait(200);
    
    for(int runId = 0; runId < 10; ++runId) {
        QVector<MeasurementRecord> records = m_dataStore->readRecords(runId);
        QCOMPARE(records.size(), 1);
        QCOMPARE(records[0].rpm, runId * 100);
    }
}