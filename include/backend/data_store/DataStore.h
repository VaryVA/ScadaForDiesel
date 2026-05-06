#pragma once

#include <string>
#include <optional>
#include <cstdint>
#include <cstddef>

// Qt includes
#include <QObject>
#include <QString>
#include <QVector>
#include <QHash>

// // #include "../DataTypes.h" // MeasurementRecord, EventRecord, Report

// Low-level layer
class Connector
{
public:
    explicit Connector(std::string path);
    virtual ~Connector() noexcept = default;

    virtual int64_t write(const std::string &data) = 0;
    virtual std::optional<std::string> read(int64_t id) const = 0;
    virtual bool update(int64_t id, const std::string &data) = 0;
    virtual bool remove(int64_t id) = 0;

    size_t size() const noexcept;
    const std::string &path() const noexcept { return path_; }

private:
    std::string path_;
};
//     size_t size_;
// };

class CSVConnector : public Connector
{
public:
    explicit CSVConnector(std::string path); // : Connector(std::move(path)) {}
    ~CSVConnector() noexcept override = default;

    int64_t write(const std::string &data) override;
    std::optional<std::string> read(int64_t id) const override;
    bool update(int64_t id, const std::string &data) override;
    bool remove(int64_t id) override;
};

// Qt layer
struct MeasurementRecord
{
    qint64 runId;
    int stage;
    qint64 timestampMs;
    double rpm, torque, dieselTemp, motorTemp, resistorTemp, dieselPressure;
    double throttle, brakeTorque;
    QString flags; // "OK", "WARNING" и т.п.
};

struct EventRecord
{
    qint64 runId, timestampMs;
    int stage;
    QString type; // "stage_change", "abort", "warning", "info"
    QString message;
};

struct Report
{
    qint64 runId;
    QString startTime, endTime;
    QString finalStatus; // "completed" или "aborted"
    // QVector<StageSummary> stages;
    QVector<EventRecord> events;
};

enum class StreamType
{
    Measurement,
    Event
};

inline uint qHash(StreamType key, uint seed = 0) noexcept
{
    return qHash(static_cast<int>(key), seed);
}

struct StorageConnector
{
    Connector *connector;
    qint64 nextId = 1;
    bool headersWritten = false;
};

class DataStore : public QObject
{
    Q_OBJECT

public:
    DataStore(Connector *measurementConnector, Connector *eventConnector, QObject *parent = nullptr);
    // В случае ошибки возвращаю пустые объекты?
    QVector<MeasurementRecord> readRecords(qint64 runId) const;
    QVector<EventRecord> readEvents(qint64 runId) const;
    Report buildReport(qint64 runId) const;

public slots:
    void writeRecord(const MeasurementRecord &record);
    void writeEvent(const EventRecord &event);

private:
    QString serializeMeasurement(const MeasurementRecord &record) const;
    QString serializeEvent(const EventRecord &event) const;

    MeasurementRecord deserializeMeasurement(const QString &line) const;
    EventRecord deserializeEvent(const QString &line) const;

    void ensureHeaders(StreamType type);
    qint64 generateId(StreamType type) noexcept;

private:
    QHash<StreamType, StorageConnector> connectors_;
};

// // Выносить ли структуры в DataTypes?
// // Нужно ли использовать исключения? (иначе работать с bool/константами, std::nullopt, возвращать пустые объекты?)
// // Ситуации для обработки:
// // 1) Чтение файла, которого нет по пути / пустого файла
// // 2) Чтение/обновление/удаление по некорректному id
// // Если файла нет и происходит запись, он будет создан
// // Нужен ли QDateTime или работать через qint64?