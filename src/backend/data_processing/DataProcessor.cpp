#include "backend/data_processing/DataProcessor.h"
// Этапы — нумерация условная, должна совпадать со StateMachine.
// state_machine.cpp использует m_currentStageIndex как int.
namespace Stage
{
    constexpr int IDLE = -1; // в state_machine.cpp m_currentStageIndex = -1 до start()
    constexpr int COLD_CRANKING = 0;
    constexpr int START_AND_WARMUP = 1;
    constexpr int HOT_NO_LOAD = 2;
    constexpr int HOT_WITH_LOAD = 3;
}
// Порог предупреждения = 90% от критического.
static constexpr double WARN_RATIO = 0.9;
DataProcessor::DataProcessor(const ModelConfig &config, QObject *parent)
    : QObject(parent), m_config(config)
{
}
void DataProcessor::onStageChanged(int newStage)
{
    m_currentStage = newStage;
}
void DataProcessor::onConfigChanged(ModelConfig config)
{
    m_config = config;
}
void DataProcessor::reset()
{
    m_alarmLatched = false;
    m_currentStage = Stage::IDLE;
}
Decision DataProcessor::processFrame(const SensorFrame &frame)
{
    Decision decision;
    // После аварии — пустое решение.
    if (m_alarmLatched)
    {
        decision.state = DiagState::Alarm_Generic;
        decision.reason = QStringLiteral("alarm latched");
        emit decisionReady(decision);
        return decision;
    }
    const int stage = frame.stage;
    // неактивные режимы — проверки выключены.
    if (stage < Stage::COLD_CRANKING)
    {
        decision.state = DiagState::Ok;
        decision.reason = QStringLiteral("inactive stage");
        emit decisionReady(decision);
        return decision;
    }
    const double maxRpm =
        (stage == Stage::COLD_CRANKING)
            ? static_cast<double>(m_config.maxRpmPrir)
            : static_cast<double>(m_config.maxRpmRun);

    DiagState worstState = DiagState::Ok;
    QString worstReason;
    int worstSeverity = 0; // 0=ok, 1=prewarn, 2=alarm

    auto promote = [&](int severity, DiagState st, const QString &rsn)
    {
        if (severity > worstSeverity)
        {
            worstSeverity = severity;
            worstState = st;
            worstReason = rsn;
        }
    };
    // обороты ДВС
    if (isParamActiveOnStage("rpm", stage))
    {
        if (frame.rpm >= maxRpm)
        {
            promote(2, DiagState::Alarm_RpmOverspeed,
                    QStringLiteral("Превышение оборотов: %1 >= %2").arg(frame.rpm).arg(maxRpm));
        }
        else if (frame.rpm >= maxRpm * WARN_RATIO)
        {
            promote(1, DiagState::PreWarn_RpmHigh,
                    QStringLiteral("Обороты приближаются к пределу: %1").arg(frame.rpm));
        }
    }
    // температура ДВС (охлаждающей жидкости)
    if (isParamActiveOnStage("dieselTemp", stage))
    {
        if (frame.dieselTemp >= m_config.maxDieselTemp)
        {
            promote(2, DiagState::Alarm_DieselOverheat,
                    QStringLiteral("Перегрев ДВС: %1").arg(frame.dieselTemp));
        }
        else if (frame.dieselTemp >= m_config.maxDieselTemp * WARN_RATIO)
        {
            promote(1, DiagState::PreWarn_DieselTempHigh,
                    QStringLiteral("ДВС близок к перегреву: %1 (требуется охлаждение)")
                        .arg(frame.dieselTemp));
        }
    }
    // температура АД
    if (isParamActiveOnStage("motorTemp", stage))
    {
        if (frame.motorTemp >= m_config.maxMotorTemp)
        {
            promote(2, DiagState::Alarm_MotorOverheat,
                    QStringLiteral("Перегрев АД: %1").arg(frame.motorTemp));
        }
        else if (frame.motorTemp >= m_config.maxMotorTemp * WARN_RATIO)
        {
            promote(1, DiagState::PreWarn_MotorTempHigh,
                    QStringLiteral("АД близок к перегреву: %1 (требуется охлаждение)")
                        .arg(frame.motorTemp));
        }
    }
    // температура балластных резисторов
    if (isParamActiveOnStage("resistorTemp", stage))
    {
        if (frame.resistorTemp >= m_config.maxResistorTemp)
        {
            promote(2, DiagState::Alarm_ResistorOverheat,
                    QStringLiteral("Перегрев балластных резисторов: %1").arg(frame.resistorTemp));
        }
        else if (frame.resistorTemp >= m_config.maxResistorTemp * WARN_RATIO)
        {
            promote(1, DiagState::PreWarn_ResistorHigh,
                    QStringLiteral("Балластные резисторы близки к перегреву: %1 (требуется охлаждение)")
                        .arg(frame.resistorTemp));
        }
    }
    // давление масла ДВС — верхняя граница
    if (isParamActiveOnStage("dieselPressureMax", stage))
    {
        if (frame.dieselPressure >= m_config.maxDieselPressure)
        {
            promote(2, DiagState::Alarm_PressureOver,
                    QStringLiteral("Превышение давления масла: %1").arg(frame.dieselPressure));
        }
        else if (frame.dieselPressure >= m_config.maxDieselPressure * WARN_RATIO)
        {
            promote(1, DiagState::PreWarn_PressureHigh,
                    QStringLiteral("Давление масла близко к верхнему пределу: %1")
                        .arg(frame.dieselPressure));
        }
    }
    // давление масла ДВС — нижняя граница
    if (isParamActiveOnStage("dieselPressureMin", stage))
    {
        const double warnLow = m_config.minDieselPressure + m_config.minDieselPressure * (1.0 - WARN_RATIO);
        if (frame.dieselPressure <= m_config.minDieselPressure)
        {
            promote(2, DiagState::Alarm_PressureUnder,
                    QStringLiteral("Падение давления масла: %1").arg(frame.dieselPressure));
        }
        else if (frame.dieselPressure <= warnLow)
        {
            promote(1, DiagState::PreWarn_PressureLow,
                    QStringLiteral("Давление масла приближается к нижнему пределу: %1")
                        .arg(frame.dieselPressure));
        }
    }
    // ---- Формирование Decision ----
    decision.state = worstState;
    decision.reason = worstReason;
    if (worstSeverity == 2)
    {
        // Авария
        m_alarmLatched = true;
        decision.controls = makeStopControls();
        emit decisionReady(decision);
        return decision;
    }
    if (worstSeverity == 1)
    {
        // предаварийное состояние
        ModelControl mc;
        switch (worstState)
        {
        case DiagState::PreWarn_RpmHigh:
            // Снижаем дроссель.
            mc.type = ControlType::Throttle;
            mc.value = 0.0;
            decision.controls.append(mc);
            break;
        case DiagState::PreWarn_ResistorHigh:
            // Снижаем тормозной момент.
            mc.type = ControlType::BrakeTorque;
            mc.value = 0.0;
            decision.controls.append(mc);
            break;
        // Перегревы ДВС / АД и проблемы с давлением:
        // включением вентиляторов в applyControls будет заниматься
        // потребитель Decision — DataProcessor возвращает только DiagState и reason
        case DiagState::PreWarn_DieselTempHigh:
        case DiagState::PreWarn_MotorTempHigh:
        case DiagState::PreWarn_PressureHigh:
        case DiagState::PreWarn_PressureLow:
        default:
            break;
        }
    }

    emit decisionReady(decision);
    return decision;
}
bool DataProcessor::isParamActiveOnStage(const QString &param, int stage) const
{
    if (stage < Stage::COLD_CRANKING)
    {
        return false;
    }
    // давление и температура ДВС — после прогрева.
    if (param == "dieselPressureMin" ||
        param == "dieselPressureMax" ||
        param == "dieselTemp")
    {
        return (stage == Stage::START_AND_WARMUP ||
                stage == Stage::HOT_NO_LOAD ||
                stage == Stage::HOT_WITH_LOAD);
    }
    // балластные резисторы — на этапе с нагрузкой.
    if (param == "resistorTemp")
    {
        return (stage == Stage::HOT_WITH_LOAD);
    }
    // rpm и motorTemp — на всех активных этапах.
    return true;
}

QVector<ModelControl> DataProcessor::makeStopControls()
{
    QVector<ModelControl> v;
    v.append({ControlType::EmergencyStop, 1.0});
    v.append({ControlType::MotorEnable, 0.0});
    v.append({ControlType::Throttle, 0.0});
    v.append({ControlType::BrakeTorque, 0.0});
    v.append({ControlType::TargetRpm, 0.0});
    return v;
}
