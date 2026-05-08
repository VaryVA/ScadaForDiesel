#include "backend/modbus_client/RegisterConverter.h"
#include <bit>

double RegisterConverter::fromRegisterWordDouble(const quint16* value)
{
    return static_cast<double>(fromRegisterWordRawValue(value, 4));
}

qint64 RegisterConverter::fromRegisterWordQint(const quint16* value)
{
    return static_cast<qint64>(fromRegisterWordRawValue(value, 4));
}

int RegisterConverter::fromRegisterWordInt(const quint16* value)
{
    return static_cast<int>(fromRegisterWordRawValue(value, 4));
}

template<typename ValueT>
QList<quint16> RegisterConverter::toRegisterWords(const ValueT& value)
{
    if (sizeof(ValueT) % sizeof(quint16) != 0)
    {
        qWarning() << "Value type must be divisible by register word";
        return {};
    }
    auto bits = std::bit_cast<std::array<quint16, sizeof(ValueT) / sizeof(quint16)>>(value);
    return QVector<quint16>(bits.begin(), bits.end());
}

quint64 RegisterConverter::fromRegisterWordRawValue(const quint16* value, qsizetype count)
{
    if (count > 4)
    {
        qWarning() << "Can't read into quint64 more than 4 registers, function returned 0";
        return 0;
    }
    quint64 res;
    for (qsizetype i = 0; i < count; ++i)
    {
        res |= static_cast<quint64>(*(value + i)) << (count - i - 1) * 16;
    }
    return res;
}