#include "backend/data_processing/DataProcessor.h"
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
    // после аварии — пустое решение
    if (m_alarmLatched)
    {
        decision.state = DiagState::Alarm_Generic;
        decision.reason = QStringLiteral("alarm latched");
        return decision;
    }
    // неактивные режимы — проверки не выполняем
    if (m_currentStage == Stage::IDLE ||
        m_currentStage == Stage::COMPLETED ||
        m_currentStage == Stage::ABORTED)
    {
        decision.state = DiagState::Ok;
        decision.reason = QStringLiteral("inactive stage");
        return decision;
    }
    // выбор наихудшего состояния (приоритет — авария)
    DiagState worstState = DiagState::Ok;
    QString worstReason;
    int worstSeverity = 0; // 0=ok, 1=пре alarm, 2=alarm
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
                    QStringLiteral("Обороты приближаются к пределу: %1").arg(frame.rpm));
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
    if (isParamActiveOnStage("resistorBalance", m_currentStage))
    {
        if (frame.resistorBalance >= m_config.maxResistorBalance)
        {
            promote(2, DiagState::Alarm_ResistorOverheat,
                    QStringLiteral("Превышение по балансировочному резистору: %1")
                        .arg(frame.resistorBalance));
        }
        else if (frame.resistorBalance >= m_config.maxResistorBalance * WARN_RATIO)
        {
            promote(1, DiagState::PreWarn_ResistorHigh,
                    QStringLiteral("Балансировочный резистор близок к пределу: %1")
                        .arg(frame.resistorBalance));
        }
    }
    // давление ДВС — верхняя граница
    if (isParamActiveOnStage("dieselPressureMax", m_currentStage))
    {
        if (frame.dieselPressure >= m_config.maxDieselPressure)
        {
            promote(2, DiagState::Alarm_PressureOver,
                    QStringLiteral("Превышение давления ДВС: %1").arg(frame.dieselPressure));
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
                    QStringLiteral("Падение давления ДВС: %1").arg(frame.dieselPressure));
        }
        else if (frame.dieselPressure <= warnLow)
        {
            promote(1, DiagState::PreWarn_PressureLow,
                    QStringLiteral("Давление ДВС приближается к нижнему пределу: %1")
                        .arg(frame.dieselPressure));
        }
    }
    decision.state = worstState;
    decision.reason = worstReason;
    if (worstSeverity == 2)
    {
        // Авария
        m_alarmLatched = true;
        decision.controls = makeStopControls();
        return decision;
    }
    if (worstSeverity == 1)
    {
        // Предаварийное состояние
        ModelControl mc;
        switch (worstState)
        {
        case DiagState::PreWarn_RpmHigh:
            // Снизить уставку дросселя (0…1).
            mc.type = ControlType::ThrottleSetpoint;
            mc.value = m_config.throttleReduceTo;
            decision.controls.append(mc);
            break;
        case DiagState::PreWarn_ResistorHigh:
            // Снизить уставку тормозного момента АД (Н·м).
            mc.type = ControlType::BrakeTorqueSetpoint;
            mc.value = m_config.brakeReduceTo;
            decision.controls.append(mc);
            break;
        // Перегревы и давление сами
        case DiagState::PreWarn_DieselTempHigh:
        case DiagState::PreWarn_MotorTempHigh:
        case DiagState::PreWarn_PressureHigh:
        case DiagState::PreWarn_PressureLow:
        default:
            break;
        }
    }
    return decision;
}
bool DataProcessor::isParamActiveOnStage(const QString &param, int stage) const
{
    if (stage == Stage::IDLE ||
        stage == Stage::COMPLETED ||
        stage == Stage::ABORTED)
    {
        return false;
    }
    if (param == "dieselPressureMin" ||
        param == "dieselPressureMax" ||
        param == "dieselTemp")
    {
        return (stage == Stage::START_AND_WARMUP ||
                stage == Stage::HOT_NO_LOAD ||
                stage == Stage::HOT_WITH_LOAD);
    }
    if (param == "resistorBalance")
    {
        return (stage == Stage::HOT_WITH_LOAD);
    }
    // rpm и motorTemp — на всех активных этапах
    return true;
}
QVector<ModelControl> DataProcessor::makeStopControls()
{
    // Аварийный стоп
    // Команды режима/опроса при аварии не отправляем
    QVector<ModelControl> v;
    v.append({ControlType::ThrottleSetpoint, 0.0});
    v.append({ControlType::BrakeTorqueSetpoint, 0.0});
    return v;
}
