#pragma once
#include <QtGlobal>
#include <QVector>

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
