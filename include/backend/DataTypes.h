// DataTypes.h (фрагмент, относящийся к DataProcessor)

#ifndef DATATYPES_H
#define DATATYPES_H
#include <QString>
#include <QVector>
#include <QMetaType>
// Control registers mapping, read/write, 1-bit
struct CoilsRegistersScheme
{
    bool fanICE;
    bool fanMotor;
    bool fanBall;
};

// Device status registers mapping, only read, 1-bit
struct DiscreteRegistersScheme
{
    bool hasFault;
    bool hasLimitViolations;
};

// Settings and variables registers mapping, read/write, 16-bit
struct HoldingRegistersScheme
{
    // Максимально допустимая температура охлаждающей жидкости
    double maxCoolantTemp;
    // Минимально допустимое давление масла
    double minDieselPressure;
    // Максимально допустимое давление масла
    double maxDieselPressure;
    // Максимально допустимая частота вращения ДВС в режиме притирки
    int maxIceRpmPrir;
    // Максимально допустимая частота вращения ДВС в режиме обкатки
    int maxIceRpmRun;
    // Максимально допустимая температура АД
    double maxMotorTemp;
    // Максимально допустимая температура балластных резисторов
    double maxBallastTemp;
    // Входная частота для АД / задание частоты АД
    double motorFrequencyInput;
    // Целевая механическая нагрузка / момент АД
    double targetMotorTorque;
    // Версия (ревизия) настроек holding-регистров
    int holdingRevision;
    // Команда на симуляцию
    uint16_t simulationCommand;
    // Запрос на симуляцию
    uint16_t simulationRequest;
    // Режим симуляции
    uint16_t simulationMode;
};

// Sensor data registers mapping, only read, 16-bit
struct InputRegistersScheme
{
    // Температура охлаждающей жидкости
    double coolantTemp;
    // Давление масла
    double oilPressure;
    // Частота вращения ДВС в режиме притирки
    int iceRpmPrir;
    // Частота вращения ДВС в режиме обкатки
    int iceRpmRun;
    // Температура АД
    double motorTemp;
    // Температура балластных резисторов
    double ballastTemp;
    // Момент АД
    double motorTorque;
    // Частота АД
    double motorFrequency;
    // Ревизия входных данных / версия источника
    int revision;
    // Ревизия источника входных данных
    int sourceInputRevision;
    // Время модели
    uint32_t modelTime;
    // Временная метка регистра Input Registers
    uint32_t inputRegistersTimestamp;
    // Состояние Input Registers
    uint16_t inputRegistersState;
    // Код неисправности
    uint16_t faultCode;
};

//Структра для задания конфигурации Modbus-клиента
struct ModbusConfig
{
    //Ip-адрес Modbus-сервера
    QString host;
    //Порт Modbus-сервера
    quint16 port = 1502;
    int pollFrequencyMs = 1000;
    int timeoutMs = 1000;
    int retries = 3;
    int unitId = 1;  // we assume one modbus device, so unitId will be ignored
    // Start address is 0 for each register type
    CoilsRegistersScheme coils;
    DiscreteRegistersScheme discrete;
    InputRegistersScheme input;
    HoldingRegistersScheme holding;
};

//Снимок датчиков
struct SensorFrame
{
    //Температура ДВС
    double dieselTemp;
    //Температура АД
    double motorTemp;
    //Температура балластных резисторов
    double resistorTemp;
    //Давление
    double dieselPressure;
    //Момент
    double torque;
    //Частота
    double rpm;
    //Метка времени в UNIX-формате
    qint64 timestampMs;
    // Этап диагностики
    int stage;
};
Q_DECLARE_METATYPE(SensorFrame)

// лимиты, передаются в конструктор DataProcessor и в IModbusBridge для записи в модель
struct ModelConfig
{
    //Максимальная температура ДВС
    double maxDieselTemp;
    //Максимальная температура АД
    double maxMotorTemp;
    //Максимальная температура балластных резисторов
    double maxResistorTemp;
    //Максимальное давление в ДВС
    double maxDieselPressure;
    //Минимальное давление в ДВС
    double minDieselPressure;
    //Общая частота оборотов (можно расширить)
    int maxRpm;
};
Q_DECLARE_METATYPE(ModelConfig)

// тип управляющего воздействия (это требует уточнения)
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
    IDLE,
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

//Структра для управления моделью фронтом
struct FrontControl
{
    //Этап эксперимента
    DiagState state;
    //Максимальная частота в зависимости от этапа(необязательный параметр на некоторых этапах - может быть не задан)
    int maxFreq;
    //Время выполнения этапа в минутах(необязательный параметр на некоторых этапах - может быть не задан)
    unsigned int time;
};

// вектор управляющих воздействий + диагностика
struct Decision
{
    QVector<ModelControl> controls;
    DiagState state = DiagState::Ok;
    QString reason; // причина (для последующей записи в БД, если надо записывать, требует уточнения)
};
Q_DECLARE_METATYPE(Decision)

//Структра для DataStore
struct Data
{
    //Снимок датчиков
    SensorFrame frame;
    //Этап эксперимента
    DiagState state;
};
#endif // DATATYPES_H