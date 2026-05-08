#include <fstream>
#include <filesystem>
#include <iostream>
#include <iomanip>

#include "backend/data_store/DataStore.h"
// #include "DataStore.h"

// === Connector ===
Connector::Connector(std::string path, size_t record_size) : path_(std::move(path)), size_(0), record_size_(record_size) {}

size_t Connector::getSize() const noexcept
{
    return size_;
}

size_t Connector::getRecordSize() const noexcept
{
    return record_size_;
}

const std::string &Connector::getPath() const noexcept
{
    return path_;
}

void Connector::setSize(size_t size) noexcept
{
    size_ = size;
}

// === FileConnector ===
size_t FileConnector::getFullRecordSize() const noexcept
{
    return getRecordSize() + 1;
}

bool FileConnector::isValidId(int64_t id) const noexcept
{
    return id >= 0 && static_cast<size_t>(id) < getSize();
}

std::streampos FileConnector::getOffset(int64_t id) const noexcept
{
    return static_cast<std::streampos>(id * getFullRecordSize());
}

std::string FileConnector::normalizeRecord(const std::string &data) const
{
    std::string fixed = data;

    if (fixed.size() < getRecordSize())
    {
        std::cerr << "[FileConnector] "
                  << "Data is smaller than record size. "
                  << "Padding will be applied.\n";

        fixed.append(getRecordSize() - fixed.size(), ' ');
    }
    else if (fixed.size() > getRecordSize())
    {
        std::cerr << "[FileConnector] "
                  << "Data is larger than record size. "
                  << "Data will be truncated.\n";

        fixed = fixed.substr(0, getRecordSize());
    }

    return fixed;
}

bool FileConnector::seekRead(std::fstream &file, std::streampos offset) const
{
    file.seekg(offset);

    if (!file)
    {
        std::cerr << "[FileConnector] "
                  << "seekg failed. Offset: "
                  << offset << '\n';

        return false;
    }

    return true;
}

bool FileConnector::seekWrite(std::fstream &file, std::streampos offset) const
{
    file.seekp(offset);

    if (!file)
    {
        std::cerr << "[FileConnector] "
                  << "seekp failed. Offset: "
                  << offset << '\n';

        return false;
    }

    return true;
}

bool FileConnector::readRecord(std::fstream &file, std::streampos offset, std::string &buffer) const
{
    if (!seekRead(file, offset))
    {
        return false;
    }

    buffer.resize(getRecordSize());
    file.read(buffer.data(), static_cast<std::streamsize>(getRecordSize()));

    if (!file)
    {
        std::cerr << "[FileConnector] "
                  << "Failed to read record. Offset: "
                  << offset << '\n';

        return false;
    }

    return true;
}

bool FileConnector::writeRecord(std::fstream &file, std::streampos offset, const std::string &data)
{
    if (!seekWrite(file, offset))
    {
        return false;
    }

    file.write(data.data(), static_cast<std::streamsize>(getRecordSize()));
    file.put(RECORD_DELIMITER);

    if (!file)
    {
        std::cerr << "[FileConnector] "
                  << "Failed to write record. Offset: "
                  << offset << '\n';

        return false;
    }

    return true;
}

// === Constructor ===
FileConnector::FileConnector(std::string path, size_t record_size) : Connector(std::move(path), record_size)
{
    std::ifstream file(getPath(), std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "[FileConnector] "
                  << "Failed to open file: "
                  << getPath() << '\n';
        setSize(0);
        return;
    }

    file.seekg(0, std::ios::end);
    size_t file_size = static_cast<size_t>(file.tellg());

    if (file_size % getFullRecordSize() != 0)
    {
        std::cerr << "[FileConnector] "
                  << "Invalid file size. "
                  << "File may be corrupted.\n";
    }

    setSize(file_size / getFullRecordSize());
}

int64_t FileConnector::write(const std::string &data)
{
    const std::string path = getPath();

    // Создаём файл, если его нет
    if (!std::filesystem::exists(path))
    {
        std::ofstream create_file(path, std::ios::binary);
        if (!create_file.is_open())
        {
            std::cerr << "[FileConnector::write] Failed to create file.\n";
            return -1;
        }
    }

    std::fstream file(path, std::ios::in | std::ios::out | std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "[FileConnector::write] Failed to open file.\n";
        return -1;
    }

    std::string fixed = normalizeRecord(data);
    int64_t id = static_cast<int64_t>(getSize());
    std::streampos offset = getOffset(id);

    if (!writeRecord(file, offset, fixed))
    {
        return -1;
    }

    setSize(getSize() + 1);

    return id;
}

// === Read ===
std::optional<std::string> FileConnector::read(int64_t id) const
{
    if (!isValidId(id))
    {
        std::cerr << "[FileConnector::read] "
                  << "Invalid id: "
                  << id << '\n';

        return std::nullopt;
    }

    std::fstream file(getPath(), std::ios::in | std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "[FileConnector::read] "
                  << "Failed to open file.\n";

        return std::nullopt;
    }

    std::string buffer;

    if (!readRecord(file, getOffset(id), buffer))
    {
        return std::nullopt;
    }

    return buffer;
}

// === Update ===

bool FileConnector::update(int64_t id, const std::string &data)
{
    if (!isValidId(id))
    {
        std::cerr << "[FileConnector::update] "
                  << "Invalid id: "
                  << id << '\n';

        return false;
    }

    std::fstream file(getPath(), std::ios::in | std::ios::out | std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "[FileConnector::update] "
                  << "Failed to open file.\n";

        return false;
    }

    std::string fixed = normalizeRecord(data);
    return writeRecord(file, getOffset(id), fixed);
}

// === Remove ===
bool FileConnector::remove(int64_t id)
{
    if (!isValidId(id))
    {
        std::cerr << "[FileConnector::remove] "
                  << "Invalid id: "
                  << id << '\n';

        return false;
    }

    std::fstream file(getPath(), std::ios::in | std::ios::out | std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "[FileConnector::remove] "
                  << "Failed to open file.\n";

        return false;
    }

    size_t last_id = getSize() - 1;

    for (size_t current_id = static_cast<size_t>(id); current_id < last_id; ++current_id)
    {
        std::string buffer;

        if (!readRecord(file, getOffset(static_cast<int64_t>(current_id + 1)), buffer))
        {
            return false;
        }

        if (!writeRecord(file, getOffset(static_cast<int64_t>(current_id)), buffer))
        {
            return false;
        }
    }

    file.close();

    try
    {
        std::filesystem::resize_file(getPath(), last_id * getFullRecordSize());
    }
    catch (const std::exception &e)
    {
        std::cerr << "[FileConnector::remove] "
                  << "resize_file failed: "
                  << e.what() << '\n';

        return false;
    }

    setSize(last_id);

    return true;
}

void printRecord(FileConnector &connector, int64_t id)
{
    auto record = connector.read(id);
}

// int main()
// {
//     constexpr size_t RECORD_SIZE = 16;

//     FileConnector connector("C:\\Users\\ilyam\\Desktop\\ScadaForDiesel\\src\\backend\\data_store\\test.File", RECORD_SIZE);

//     std::cout << "Initial size: " << connector.getSize() << "\n\n";

//     // === WRITE ===
//     std::cout << "=== WRITE ===\n";

//     int64_t id0 = connector.write("Hello");
//     int64_t id1 = connector.write("World");
//     int64_t id2 = connector.write("VeryVeryLongString123");

//     std::cout << "Inserted ids: "
//               << id0 << ", "
//               << id1 << ", "
//               << id2 << "\n";

//     std::cout << "Size after write: "
//               << connector.getSize() << "\n\n";

//     // === READ ===
//     std::cout << "=== READ ===\n";

//     printRecord(connector, id0);
//     printRecord(connector, id1);
//     printRecord(connector, id2);

//     std::cout << "\n";

//     // === UPDATE ===
//     std::cout << "=== UPDATE ===\n";

//     bool updated = connector.update(id1, "Updated");

//     std::cout << "Update result: "
//               << std::boolalpha
//               << updated << "\n";

//     printRecord(connector, id1);

//     std::cout << "\n";

//     // === REMOVE ===
//     std::cout << "=== REMOVE ===\n";

//     bool removed = connector.remove(id0);

//     std::cout << "Remove result: "
//               << std::boolalpha
//               << removed << "\n";

//     std::cout << "Size after remove: "
//               << connector.getSize() << "\n";

//     std::cout << "\nRecords after remove:\n";

//     for (size_t i = 0; i < connector.getSize(); ++i)
//     {
//         printRecord(connector, static_cast<int64_t>(i));
//     }

//     std::cout << "\n";

//     // === INVALID ACCESS ===
//     std::cout << "=== INVALID ACCESS ===\n";

//     printRecord(connector, 999);

//     bool invalidUpdate = connector.update(999, "Test");
//     bool invalidRemove = connector.remove(999);

//     std::cout << "Invalid update: "
//               << invalidUpdate << "\n";

//     std::cout << "Invalid remove: "
//               << invalidRemove << "\n";

//     return 0;
// }