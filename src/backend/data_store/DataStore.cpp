#include "backend/data_store/DataStore.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

// Connector implementation
Connector::Connector(std::string path) : path_(std::move(path)) {}

size_t Connector::size() const noexcept {
    QFile file(QString::fromStdString(path_));
    return file.size();
}

// CSVConnector implementation
CSVConnector::CSVConnector(std::string path) : Connector(std::move(path)) {}

int64_t CSVConnector::write(const std::string &data) {
    QFile file(QString::fromStdString(path()));
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Cannot open file for writing:" << file.fileName();
        return -1;
    }
    QTextStream out(&file);
    out << QString::fromStdString(data) << "\n";
    return file.size();
}

std::optional<std::string> CSVConnector::read(int64_t id) const {
    QFile file(QString::fromStdString(path()));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file for reading:" << file.fileName();
        return std::nullopt;
    }
    QTextStream in(&file);
    QString line;
    int currentId = 1;
    while (in.readLineInto(&line)) {
        if (currentId == id) {
            return line.toStdString();
        }
        ++currentId;
    }
    return std::nullopt;
}

bool CSVConnector::update(int64_t id, const std::string &data) {
    // Stub: not implemented
    return false;
}

bool CSVConnector::remove(int64_t id) {
    // Stub: not implemented
    return false;
}

// DataStore implementation
DataStore::DataStore(Connector *measurementConnector, Connector *eventConnector, QObject *parent)
    : QObject(parent) {
    connectors_[StreamType::Measurement] = {measurementConnector};
    connectors_[StreamType::Event] = {eventConnector};
}

QVector<MeasurementRecord> DataStore::readRecords(qint64 runId) const {
    // Stub
    return {};
}

QVector<EventRecord> DataStore::readEvents(qint64 runId) const {
    // Stub
    return {};
}

Report DataStore::buildReport(qint64 runId) const {
    // Stub
    return {};
}

void DataStore::writeRecord(const MeasurementRecord &record) {
    // Stub
}

void DataStore::writeEvent(const EventRecord &event) {
    // Stub
}

QString DataStore::serializeMeasurement(const MeasurementRecord &record) const {
    // Stub
    return {};
}

QString DataStore::serializeEvent(const EventRecord &event) const {
    // Stub
    return {};
}

MeasurementRecord DataStore::deserializeMeasurement(const QString &line) const {
    // Stub
    return {};
}

EventRecord DataStore::deserializeEvent(const QString &line) const {
    // Stub
    return {};
}

void DataStore::ensureHeaders(StreamType type) {
    // Stub
}

qint64 DataStore::generateId(StreamType type) noexcept {
    // Stub
    return 1;
}
