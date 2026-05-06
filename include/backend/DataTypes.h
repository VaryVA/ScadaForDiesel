#pragma once
#include <QtGlobal>
#include <QString>
#include <QVector>

// Control registers mapping, read/write, 1-bit
struct CoilsRegistersScheme
{
    bool motorEnabled;
};

// Device status registers mapping, only read, 1-bit
struct DiscreteRegistersScheme
{
    bool fanAd;
    bool fanBall;
};

// Settings and variables registers mapping, read/write, 16-bit
struct HoldingRegistersScheme
{
    double targetRpm;
    double throttlePosition;
    double brakeTorque;
};

// Sensor data registers mapping, only read, 16-bit
struct InputRegistersScheme
{
    double rpm, torque;
    double dieselTemp, motorTemp, resistorTemp, dieselPressure;
    double throttle, brakeTorque;
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

//Этапы эксперимента
enum DiagState
{
    IDLE = 0,
    COLD_CRANKING,
    START_AND_WARMUP,
    HOT_NO_LOAD,
    HOT_WITH_LOAD,
    COMPLETED,
    ABORTED
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

//Структра для задания максимальных значений модели
struct ModelConfig
{
    //Максимальная температура ДВС
    double maxDieselTemp;
    //Максимальная температура АД
    double maxAdTemp;
    //Максимальная температура балластных резисторов
    double maxResistorTemp;
    //Максимальное давление в ДВС
    double maxDieselPressure;
    //Минимальное давление в ДВС
    double minDieselPressure;
};

//Параметры модели, которые могут быть изменены в ходе эксперимента
enum DecisionType
{
    //Максимальная частота в режиме притирки
    MAX_FREQ_LAP,
    //Максимальная частота в режиме обкатки
    MAX_FREQ_HOT,
    //Состояние вентилятора ассинхронного двигателя
    FAN_AD,
    //Состояние вентилятора на балластных резисторах
    FAN_RESISTOR
};

//Управляющее воздействие
struct ModelControl
{
    //Тип изменяемого параметра
    DecisionType decisionType;
    //Значение (Для FAN_AD и FAN_RESISTOR - значение 0(false), значение 1(true))
    int value;
};

struct Decision
{
    //Вектор управляющих воздействий
    QVector<ModelControl> controls;
    //Этап эксперимента
    DiagState state;
};

//Снимок датчиков
struct SensorFrame
{
    //Метка времени в UNIX-формате
    qint64 timestampSec;
    //Температура ДВС
    double dieselTemp;
    //Температура АД
    double adTemp;
    //Температура балластных резисторов
    double resistorTemp;
    //Давление
    double dieselPressure;
    //Момент
    double moment;
    //Частота
    double freq;
};

//Структра для DataStore
struct Data
{
    //Снимок датчиков
    SensorFrame frame;
    //Этап эксперимента
    DiagState state;
};


