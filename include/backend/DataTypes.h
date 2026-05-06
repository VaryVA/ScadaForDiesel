// DataTypes.h (фрагмент, относящийся к DataProcessor)

#ifndef DATATYPES_H
#define DATATYPES_H
#include <QString>
#include <QVector>
#include <QMetaType>
// пакет измерений  то, что приходит от ModbusClient через StateMachine (все требует уточнения)
struct SensorFrame
{
    double dieselTemp;      //< температура ДВС
    double motorTemp;       //< температура АД (асинхронного двигателя)
    double resistorBalance; //< балансировочный резистор (?)
    double dieselPressure;  //< давление ДВС
    double torque;          //< момент
    double rpm;             //< частота вращения
    qint64 timestampMs;     //< метка времени
    int stage;              //< № этапа
};
Q_DECLARE_METATYPE(SensorFrame)
// лимиты, передаются в DataProcessor в конструкторе (тоже требует уточнения)
struct ModelConfig
{
    double maxDieselTemp;
    double maxMotorTemp;
    double maxResistorBalance;
    double maxDieselPressure;
    double minDieselPressure;
    double maxRpm; //< общий лимит оборотов (можно расширить)
};
Q_DECLARE_METATYPE(ModelConfig)
// тип управляющего воздействия (и это требует уточнения)
enum class ControlType
{
    None,
    Throttle,     // дроссель
    BrakeTorque,  // тормозной момент
    TargetRpm,    // целевые обороты
    MotorEnable,  // вкл/выкл мотора
    EmergencyStop // аварийный стоп
};
// одно управляющее воздействие (тип + значение)
struct ModelControl
{
    ControlType type = ControlType::None;
    double value = 0.0;
};
Q_DECLARE_METATYPE(ModelControl)
// диагностическое состояние модели (добавлены предаварийные)
enum class DiagState
{
    Ok,
    PreWarn_RpmHigh,
    PreWarn_DieselTempHigh,
    PreWarn_MotorTempHigh,
    PreWarn_ResistorHigh,
    PreWarn_PressureHigh,
    PreWarn_PressureLow,
    Alarm_RpmOverspeed,
    Alarm_DieselOverheat,
    Alarm_MotorOverheat,
    Alarm_ResistorOverheat,
    Alarm_PressureOver,
    Alarm_PressureUnder,
    Alarm_Generic
};
// вектор управляющих воздействий + диагностика
struct Decision
{
    QVector<ModelControl> controls;
    DiagState state = DiagState::Ok;
    QString reason; // причина (для последующей записи в БД, если надо записывать, требует уточнения)
};
Q_DECLARE_METATYPE(Decision)
#endif // DATATYPES_H