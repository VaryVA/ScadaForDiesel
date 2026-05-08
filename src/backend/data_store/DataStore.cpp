#include <fstream>
#include <filesystem>
#include <iostream>
#include <iomanip>

#include "backend/data_store/DataStore.h"

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
FileConnector::FileConnector(std::string path, size_t record_size) : Connector(std::move(path), record_size)
{
    std::ifstream file(getPath(), std::ios::binary);

    if (!file.is_open())
    {
        setSize(0);
        return;
    }

    file.seekg(0, std::ios::end);
    size_t file_size = static_cast<size_t>(file.tellg());

    setSize(file_size / getRecordSize());
}

// O(1)
int64_t FileConnector::write(const std::string &data)
{
    std::ofstream file(getPath(), std::ios::app);

    if (!file.is_open())
    {
        return -1;
    }

    std::string fixed = data;
    if (fixed.size() < getRecordSize())
    {
        fixed.append(getRecordSize() - fixed.size(), ' ');
    }
    else if (fixed.size() > getRecordSize())
    {
        fixed = fixed.substr(0, getRecordSize());
    }

    file.write(fixed.data(), getRecordSize());
    file.put('\n');

    int64_t id = static_cast<int64_t>(getSize());
    setSize(getSize() + 1);

    return id;
}

// O(1)
std::optional<std::string> FileConnector::read(int64_t id) const
{
    if (id < 0 || static_cast<size_t>(id) >= getSize())
    {
        return std::nullopt;
    }

    std::ifstream file(getPath(), std::ios::binary);

    if (!file.is_open())
    {
        return std::nullopt;
    }

    std::streampos offset = static_cast<std::streampos>(id * (getRecordSize() + 1)); // + '\n'

    file.seekg(offset);

    std::string buffer(getRecordSize(), '\0');
    file.read(buffer.data(), getRecordSize());

    return buffer;
}

// O(1)
bool FileConnector::update(int64_t id, const std::string &data)
{
    if (id < 0 || static_cast<size_t>(id) >= getSize())
    {
        return false;
    }

    std::fstream file(getPath(), std::ios::in | std::ios::out | std::ios::binary);

    if (!file.is_open())
    {
        return false;
    }

    std::string fixed = data;

    if (fixed.size() < getRecordSize())
    {
        fixed.append(getRecordSize() - fixed.size(), ' ');
    }
    else if (fixed.size() > getRecordSize())
    {
        fixed = fixed.substr(0, getRecordSize());
    }

    std::streampos offset = static_cast<std::streampos>(id * (getRecordSize() + 1));

    file.seekp(offset);
    file.write(fixed.data(), getRecordSize());

    return true;
}

// O(n)
bool FileConnector::remove(int64_t id)
{
    if (id < 0 || static_cast<size_t>(id) >= getSize())
    {
        return false;
    }

    std::fstream file(getPath(), std::ios::in | std::ios::out | std::ios::binary);

    if (!file.is_open())
    {
        return false;
    }

    size_t record_full_size = getRecordSize() + 1; // + '\n'
    size_t last_id = getSize() - 1;

    // Сдвигаем все записи после удаляемой на одну позицию влево
    for (size_t current_id = static_cast<size_t>(id); current_id < last_id; ++current_id)
    {
        std::streampos read_offset = static_cast<std::streampos>((current_id + 1) * record_full_size);
        std::streampos write_offset = static_cast<std::streampos>(current_id * record_full_size);

        // Читаем следующую запись
        file.seekg(read_offset);
        std::string buffer(getRecordSize(), '\0');
        file.read(buffer.data(), getRecordSize());

        if (!file)
        {
            return false;
        }

        // Пишем её на место предыдущей
        file.seekp(write_offset);
        file.write(buffer.data(), getRecordSize());
        file.put('\n');

        if (!file)
        {
            return false;
        }
    }

    file.close();

    uintmax_t new_size = static_cast<uintmax_t>(last_id * record_full_size);
    std::filesystem::resize_file(getPath(), new_size);
    setSize(last_id);

    return true;
}


void printRecord(FileConnector& connector, int64_t id)
{
    auto record = connector.read(id);

    if (record.has_value())
    {
        std::cout << "Record [" << id << "] = '" << *record << "'\n";
    }
    else
    {
        std::cout << "Record [" << id << "] not found\n";
    }
}

// int main()
// {
//     constexpr size_t RECORD_SIZE = 16;

//     FileConnector connector("C:\\Users\\ilyam\\Desktop\\ScadaForDiesel\\src\\backend\\data_store\\test.File", RECORD_SIZE);

//     std::cout << "Initial size: " << connector.getSize() << "\n\n";

//     // === WRITE ===
//     std::cout << "=== WRITE ===\n";

//     int64_t id1 = connector.write("Hello");
//     int64_t id2 = connector.write("World");
//     int64_t id3 = connector.write("VeryVeryLongString123");

//     std::cout << "Inserted ids: "
//               << id1 << ", "
//               << id2 << ", "
//               << id3 << "\n";

//     std::cout << "Size after write: "
//               << connector.getSize() << "\n\n";

//     // === READ ===
//     std::cout << "=== READ ===\n";

//     printRecord(connector, id1);
//     printRecord(connector, id2);
//     printRecord(connector, id3);

//     std::cout << "\n";

//     // === UPDATE ===
//     std::cout << "=== UPDATE ===\n";

//     bool updated = connector.update(id2, "Updated");

//     std::cout << "Update result: "
//               << std::boolalpha
//               << updated << "\n";

//     printRecord(connector, id2);

//     std::cout << "\n";

//     // === REMOVE ===
//     std::cout << "=== REMOVE ===\n";

//     bool removed = connector.remove(id1);

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