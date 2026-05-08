#ifndef DATASTORE_H
#define DATASTORE_H

#include <string>
#include <optional>

#include <QObject>

#include "backend/DataTypes.h" // Data

// Low-level layer
class Connector
{
public:
    Connector(std::string path, size_t record_size);
    virtual ~Connector() noexcept = default;

    virtual int64_t write(const std::string &data) = 0;
    virtual std::optional<std::string> read(int64_t id) const = 0;
    virtual bool update(int64_t id, const std::string &data) = 0;
    virtual bool remove(int64_t id) = 0;

    size_t getSize() const noexcept;
    const std::string &getPath() const noexcept;
    size_t getRecordSize() const noexcept;

protected:
    void setSize(size_t size) noexcept;

private:
    std::string path_;
    size_t size_;
    size_t record_size_;
};

class FileConnector : public Connector
{
public:
    FileConnector(std::string path, size_t record_size);
    ~FileConnector() noexcept override = default;

    int64_t write(const std::string &data) override;
    std::optional<std::string> read(int64_t id) const override;
    bool update(int64_t id, const std::string &data) override;
    bool remove(int64_t id) override;
};

// Qt layer
class DataStore : public QObject
{
    Q_OBJECT

public:
    DataStore(Connector *connector, QObject *parent = nullptr);
    QVector<Data> readData(qint64 id) const; // В случае ошибок возвращает пустой вектор
    QVector<Data> readAllData() const;       // В случае ошибок возвращает пустой вектор

public slots:
    bool writeData(const Data &record);

private:
    QString serializeData(const Data &record) const;
    Data deserializeData(const QString &line) const;

    void ensureHeaders();
    qint64 generateId() noexcept;

private:
    Connector *connector_;
    bool headersEnsured_;
};

#endif // DATASTORE_H