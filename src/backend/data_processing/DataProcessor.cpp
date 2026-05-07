#include "backend/data_processing/DataProcessor.h"
// этапы — должны совпадать с StateMachine (согласовать)
namespace Stage
{
    constexpr int IDLE = 0;
    constexpr int COLD_CRANKING = 1;
    constexpr int START_AND_WARMUP = 2;
    constexpr int HOT_NO_LOAD = 3;
    constexpr int HOT_WITH_LOAD = 4;
    constexpr int COMPLETED = 5;
    constexpr int ABORTED = 6;
}
// доля от критического порога, при которой выдаём предаварийное состояние (тоже согласовать)
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
void DataProcessor::processFrame(SensorFrame frame)
{
    // после аварии решения больше не формируем
    if (m_alarmLatched)
    {
        return;
    }
    // в неактивных режимах проверки нет
    if (m_currentStage == Stage::IDLE ||
        m_currentStage == Stage::COMPLETED ||
        m_currentStage == Stage::ABORTED)
    {
        Decision d;
        d.state = DiagState::Ok;
        d.reason = QStringLiteral("inactive stage");
        emit decisionReady(d);
        return;
    }
    // будем выбирать наихудшее состояние (приоритет — авария)
    DiagState worstState = DiagState::Ok;
    QString worstReason;
    int worstSeverity = 0; // 0 = ok, 1 = пред alarm, 2 = alarm
    auto promote = [&](int severity, DiagState st, const QString &rsn)
    {
        if (severity > worstSeverity)
        {
            worstSeverity = severity;
            worstState = st;
            worstReason = rsn;
        }
    };
    // обороты
    if (isParamActiveOnStage("rpm", m_currentStage))
    {
        if (frame.rpm >= m_config.maxRpmPrir)
        {
            promote(2, DiagState::Alarm_RpmOverspeed,
                    QStringLiteral("Превышение оборотов: %1 >= %2")
                        .arg(frame.rpm)
                        .arg(m_config.maxRpmPrir));
        }
        else if (frame.rpm >= m_config.maxRpmPrir * WARN_RATIO)
        {
            promote(1, DiagState::PreWarn_RpmHigh,
                    QStringLiteral("Обороты приближаются к пределу: %1")
                        .arg(frame.rpm));
        }
    }
    // температура ДВС
    if (isParamActiveOnStage("dieselTemp", m_currentStage))
    {
        if (frame.dieselTemp >= m_config.maxDieselTemp)
        {
            promote(2, DiagState::Alarm_DieselOverheat,
                    QStringLiteral("Перегрев ДВС: %1").arg(frame.dieselTemp));
        }
        else if (frame.dieselTemp >= m_config.maxDieselTemp * WARN_RATIO)
        {
            promote(1, DiagState::PreWarn_DieselTempHigh,
                    QStringLiteral("ДВС близок к перегреву: %1").arg(frame.dieselTemp));
        }
    }
    // температура АД
    if (isParamActiveOnStage("motorTemp", m_currentStage))
    {
        if (frame.motorTemp >= m_config.maxMotorTemp)
        {
            promote(2, DiagState::Alarm_MotorOverheat,
                    QStringLiteral("Перегрев АД: %1").arg(frame.motorTemp));
        }
        else if (frame.motorTemp >= m_config.maxMotorTemp * WARN_RATIO)
        {
            promote(1, DiagState::PreWarn_MotorTempHigh,
                    QStringLiteral("АД близок к перегреву: %1").arg(frame.motorTemp));
        }
    }
    // балансировочный резистор
    if (isParamActiveOnStage("resistorTemp", m_currentStage))
    {
        if (frame.resistorTemp >= m_config.maxResistorTemp)
        {
            promote(2, DiagState::Alarm_ResistorOverheat,
                    QStringLiteral("Превышение по балансировочному резистору: %1")
                        .arg(frame.resistorTemp));
        }
        else if (frame.resistorTemp >= m_config.maxResistorTemp * WARN_RATIO)
        {
            promote(1, DiagState::PreWarn_ResistorHigh,
                    QStringLiteral("Балансировочный резистор близок к пределу: %1")
                        .arg(frame.resistorTemp));
        }
    }
    // давление ДВС — верхняя граница
    if (isParamActiveOnStage("dieselPressureMax", m_currentStage))
    {
        if (frame.dieselPressure >= m_config.maxDieselPressure)
        {
            promote(2, DiagState::Alarm_PressureOver,
                    QStringLiteral("Превышение давления ДВС: %1")
                        .arg(frame.dieselPressure));
        }
        else if (frame.dieselPressure >= m_config.maxDieselPressure * WARN_RATIO)
        {
            promote(1, DiagState::PreWarn_PressureHigh,
                    QStringLiteral("Давление ДВС близко к верхнему пределу: %1")
                        .arg(frame.dieselPressure));
        }
    }
    // давление ДВС — нижняя граница
    if (isParamActiveOnStage("dieselPressureMin", m_currentStage))
    {
        const double warnLow = m_config.minDieselPressure + m_config.minDieselPressure * (1.0 - WARN_RATIO);
        if (frame.dieselPressure <= m_config.minDieselPressure)
        {
            promote(2, DiagState::Alarm_PressureUnder,
                    QStringLiteral("Падение давления ДВС: %1")
                        .arg(frame.dieselPressure));
        }
        else if (frame.dieselPressure <= warnLow)
        {
            promote(1, DiagState::PreWarn_PressureLow,
                    QStringLiteral("Давление ДВС приближается к нижнему пределу: %1")
                        .arg(frame.dieselPressure));
        }
    }
    Decision decision;
    decision.state = worstState;
    decision.reason = worstReason;
    if (worstSeverity == 2)
    {
        // АВАРИЯ
        m_alarmLatched = true;
        decision.controls = makeStopControls();
        emit decisionReady(decision);
        return;
    }
    if (worstSeverity == 1)
    {
        // предаварийное состояние — корректирующее воздействие
        ModelControl mc;
        switch (worstState)
        {
        case DiagState::PreWarn_RpmHigh:
            mc.type = ControlType::Throttle;
            mc.value = 0.0; // прикрыть дроссель (значение согласовать)
            decision.controls.append(mc);
            break;
        case DiagState::PreWarn_ResistorHigh:
            mc.type = ControlType::BrakeTorque;
            mc.value = 0.0; // снизить тормозной момент
            decision.controls.append(mc);
            break;
        // Перегревы и давление пока не доделаны
        case DiagState::PreWarn_DieselTempHigh:
        case DiagState::PreWarn_MotorTempHigh:
        case DiagState::PreWarn_PressureHigh:
        case DiagState::PreWarn_PressureLow:
        default:
            break;
        }
    }
    emit decisionReady(decision);
}
bool DataProcessor::isParamActiveOnStage(const QString &param, int stage) const
{
    if (stage == Stage::IDLE ||
        stage == Stage::COMPLETED ||
        stage == Stage::ABORTED)
    {
        return false;
    }
    // давление и температура ДВС — после запуска
    if (param == "dieselPressureMin" ||
        param == "dieselPressureMax" ||
        param == "dieselTemp")
    {
        return (stage == Stage::START_AND_WARMUP ||
                stage == Stage::HOT_NO_LOAD ||
                stage == Stage::HOT_WITH_LOAD);
    }
    // балансировочный резистор — на этапе с нагрузкой
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