// DataTypes.h (фрагмент, относящийся к DataProcessor)

#ifndef DATATYPES_H
#define DATATYPES_H
#include <QString>
#include <QVector>
#include <QMetaType>
// InputRegisters offset (sensors data, only read operations)
namespace InputRegisters
{
    static constexpr qsizetype count = 50;
    // Температура охлаждающей жидкости
    static constexpr uint16_t T_cool = 0;
    // Давление масла
    static constexpr uint16_t P_oil = 4;
    // Частота вращения ДВС в режиме притирки
    static constexpr uint16_t omega_ICE_prir = 8;
    // Частота вращения ДВС в режиме обкатки
    static constexpr uint16_t omega_ICE_run = 12;
    // Температура АД
    static constexpr uint16_t T_AD = 16;
    // Температура балластных резисторов
    static constexpr uint16_t T_ballast = 20;
    // Момент АД
    static constexpr uint16_t M_AD = 24;
    // Частота АД
    static constexpr uint16_t f_AD = 28;
    // Ревизия входных данных / версия источника
    static constexpr uint16_t revision = 32;
    // Ревизия источника входных данных
    static constexpr uint16_t sourceInputRevision = 36;
    // Время модели
    static constexpr uint16_t modelTime = 40;
    // Временная метка регистра Input Registers
    static constexpr uint16_t timestamp_ir = 44;
    // Состояние Input Registers
    static constexpr uint16_t state_ir = 48;
    // Код неисправности
    static constexpr uint16_t faultCode = 49;
}

// HoldingRegisters offset (settings and variables, read/write operations)
namespace HoldingRegisters
{
    static constexpr qsizetype count = 59;
    // Максимально допустимая температура охлаждающей жидкости
    static constexpr uint16_t T_cool_max = 0;
    // Минимально допустимое давление масла
    static constexpr uint16_t P_oil_min = 4;
    // Максимально допустимое давление масла
    static constexpr uint16_t P_oil_max = 8;
    // Максимально допустимая частота вращения ДВС в режиме притирки
    static constexpr uint16_t omega_ICE_max_prir = 12;
    // Максимально допустимая частота вращения ДВС в режиме обкатки
    static constexpr uint16_t omega_ICE_max_run = 16;
    // Лишний регистр
    static constexpr uint16_t rpm_max_lapping = 20;
    // Лишний регистр
    static constexpr uint16_t rpm_max_run = 24;
    // Лишний регистр
    static constexpr uint16_t target_brake_torque_nm = 28;
    static constexpr uint16_t throttle_position = 32;
    // Максимально допустимая температура АД
    static constexpr uint16_t T_AD_max = 36;
    // Максимально допустимая температура балластных резисторов
    static constexpr uint16_t T_ballast_max = 40;
    // Входная частота для АД / задание частоты АД
    static constexpr uint16_t f_AD_Input = 44;
    // Целевая механическая нагрузка / момент АД  
    static constexpr uint16_t M_AD_target = 48;
    // Версия (ревизия) настроек holding-регистров
    static constexpr uint16_t revision_h = 52;
    // Команда на симуляцию
    static constexpr uint16_t simulationCommand = 56;
    // Запрос на симуляцию
    static constexpr uint16_t simulationRequest = 57;
    // Режим симуляции
    static constexpr uint16_t simulationMode = 58;
}

// Coils offset (control registers, read/write operations)
namespace CoilsRegisters
{
    static constexpr uint16_t count = 3;
    static constexpr uint16_t fan_ICE = 0;
    static constexpr uint16_t fan_AD = 1;
    static constexpr uint16_t fan_ballast = 2;
}

// Discrete Inputs offset (device status, only read operations)
namespace DiscreteRegisters
{
    static constexpr uint16_t count = 2;
    static constexpr uint16_t hasFault = 0;
    static constexpr uint16_t hasLimitViolations = 1;
}

//Структра для задания конфигурации Modbus-клиента
struct ModbusConfig
{
    //Ip-адрес Modbus-сервера
    QString host = "127.0.0.1";
    //Порт Modbus-сервера
    quint16 port = 1502;
    int pollFrequencyMs = 1000;
    int timeoutMs = 1000;
    int retries = 3;
    int unitId = 1;  // we assume one modbus device, so unitId will be ignored
};

//Снимок датчиков
struct SensorFrame
{
    //Температура ДВС(температура охлаждающей жидкости)
    double dieselTemp;
    //Температура АД
    double motorTemp;
    //Температура балластных резисторов
    double resistorTemp;
    //Давление масла
    double dieselPressure;
    //Момент АД
    double torque;
    //Частота вращения ДВС
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
    //Максимальная температура ДВС(температура охлаждающей жидкости)
    double maxDieselTemp;
    //Максимальная температура АД
    double maxMotorTemp;
    //Максимальная температура балластных резисторов
    double maxResistorTemp;
    //Максимальное давление масла в ДВС
    double maxDieselPressure;
    //Минимальное давление масла в ДВС
    double minDieselPressure;
    //Общая частота оборотов в режиме притирки
    int maxRpmPrir;
    //Общая частота оборотов в режиме обкатки
    int maxRpmRun;
    //Частота АД
    double freqAD;
    //Момент АД
    double momentAD;
};
Q_DECLARE_METATYPE(ModelConfig)


//Команды на симуляцию
enum class SimulationCommand : uint8_t
{
    None,
    Start,
    Stop,
    Reset,
    EmergencyStop
};

//Запрос на симуляцию
enum class SimulationRequest : uint8_t
{
    None,
    ReadCurrentState,
    StepAndRead
};

//Режим симуляции
enum class SimulationMode : uint8_t
{
    ColdRun,
    StartWarmup,
    HotNoLoad,
    HotLoad
};

// тип управляющего воздействия (это требует уточнения)
enum class ControlType
{
    None,
    SimulationCommand, // Команда на симуляцию (simulationCommand)
    SimulationRequest, // Запрос на симуляцию (simulationRequest)
    SimulationMode,    // Режим симуляции (simulationMode)
    Fan_ICE,
    Fan_AD,
    Fan_Ballast
};

// одно управляющее воздействие (тип + значение)
struct ModelControl
{
    ControlType type = ControlType::None;
    uint8_t value = 0;
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
    //Время обкатки ДВС в режиме притирки(в минутах)
    unsigned int timePrir;
    //Время горячей обкатки ДВС(в минутах)
    unsigned int timeHot;
    //Время горячей обкатки ДВС с нагрузкой(в минутах)
    unsigned int timeHotWithLoad;
    //Структура для задания минимальных/максимальных значений в модели
    ModelConfig config;
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
