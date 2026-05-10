#pragma once
#include <QtGlobal>
#include <QVector>
#include <QDebug>
#include <bit>
#include <array>

class RegisterConverter
{
public:
    double fromRegisterWordDouble(const quint16* value);
    qint64 fromRegisterWordQint(const quint16* value);
    int fromRegisterWordInt(const quint16* value);
    template<typename ValueT>
    QList<quint16> toRegisterWords(const ValueT& value);
private:
    quint64 fromRegisterWordRawValue(const quint16* value, qsizetype count);
};

template<typename ValueT>
QList<quint16> RegisterConverter::toRegisterWords(const ValueT& value)
{
    if (sizeof(ValueT) % sizeof(quint16) != 0)
    {
        qWarning() << "Value type must be divisible by register word";
        return {};
    }
    auto bits = std::bit_cast<std::array<quint16, sizeof(ValueT) / sizeof(quint16)>>(value);
    return QList<quint16>(bits.begin(), bits.end());
}
