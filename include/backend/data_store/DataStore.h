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
    size_t getRecordSize() const noexcept;
    const std::string &getPath() const noexcept;

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
    FileConnector(std::string path, size_t record_size); // Необходимо определить макрос для размера одной записи (128 байт хватит); #define RECORD_SIZE 128
    ~FileConnector() noexcept override = default;

    int64_t write(const std::string &data) override;
    std::optional<std::string> read(int64_t id) const override;
    bool update(int64_t id, const std::string &data) override;
    bool remove(int64_t id) override;

private:
    static constexpr char RECORD_DELIMITER = '\n';

    size_t getFullRecordSize() const noexcept;
    bool isValidId(int64_t id) const noexcept;
    std::streampos getOffset(int64_t id) const noexcept;
    std::string normalizeRecord(const std::string &data) const;

    bool seekRead(std::fstream &file, std::streampos offset) const;
    bool seekWrite(std::fstream &file, std::streampos offset) const;
    bool readRecord(std::fstream &file, std::streampos offset, std::string &buffer) const;
    bool writeRecord(std::fstream &file, std::streampos offset, const std::string &data);
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
    QString serializeData(qint64 id, const Data &record, int precision = 3) const;
    bool deserializeData(const QString &line, Data &data) const;
    qint64 generateId() noexcept;

private:
    Connector *connector_;
};

#endif // DATASTORE_H