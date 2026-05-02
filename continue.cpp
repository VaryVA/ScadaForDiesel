#include "DataProcessor.h"
#include <QDebug>
#include <cmath>

// Номера этапов должны совпадать:
// 0 IDLE, 1 COLD_CRANKING, 2 START_AND_WARMUP,
// 3 HOT_NO_LOAD, 4 HOT_WITH_LOAD, 5 COMPLETED, 6 ABORTED
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
// % от критического порога, при достижении которой выдаём предупреждение
static constexpr double WARN_RATIO = 0.9;
DataProcessor::DataProcessor(QObject *parent)
    : QObject(parent)
{
}
void DataProcessor::setConfig(const Config &config)
{
    m_config = config;
    m_configured = true;
}
void DataProcessor::onStageChanged(int newStage)
{
    m_currentStage = newStage;
    // при переходе в COMPLETED/ABORTED дальнейшие проверки не нужны,
    //  но решение об этом принимает другой этап, здесь по лгике надо престать слать кадры
}
void DataProcessor::reset()
{
    m_alarmLatched = false;
    m_currentStage = Stage::IDLE;
    m_lastFrameTsMs = 0;
}
void DataProcessor::processFrame(SensorFrame frame)
{
    if (!m_configured)
    {
        qWarning() << "[DataProcessor] frame received before config; dropping";
        return;
    }
    // после аварии блокируем формирование новых решений
    if (m_alarmLatched)
    {
        return;
    }
    m_lastFrameTsMs = frame.timestampMs;
    // в режимах  IDLE COMPLETED ABORTED  проверки не нужны
    if (m_currentStage == Stage::IDLE ||
        m_currentStage == Stage::COMPLETED ||
        m_currentStage == Stage::ABORTED)
    {
        Decision d;
        d.status = Status::OK;
        d.message = QStringLiteral("inactive stage");
        emit decisionReady(d);
        return;
    }
    // выбор максимально допустимых оборотов в зависимости от этапа.
    const double maxRpm = (m_currentStage == Stage::COLD_CRANKING)
                              ? m_config.maxRpmCold
                              : m_config.maxRpmHot;
    // сборка набора проверок
    QVector<Check> checks;
    if (isParameterActiveOnStage("rpm", m_currentStage))
    {
        checks.push_back({"rpm",
                          frame.rpm,
                          maxRpm * WARN_RATIO,
                          maxRpm,
                          true,
                          QStringLiteral("Превышение частоты вращения")});
    }
    if (isParameterActiveOnStage("dieselTemp", m_currentStage))
    {
        checks.push_back({"dieselTemp",
                          frame.dieselTemp,
                          m_config.maxDieselTemp * WARN_RATIO,
                          m_config.maxDieselTemp,
                          true,
                          QStringLiteral("Перегрев дизеля")});
    }
    if (isParameterActiveOnStage("motorTemp", m_currentStage))
    {
        checks.push_back({"motorTemp",
                          frame.motorTemp,
                          m_config.maxMotorTemp * WARN_RATIO,
                          m_config.maxMotorTemp,
                          true,
                          QStringLiteral("Перегрев электродвигателя")});
    }
    if (isParameterActiveOnStage("resistorTemp", m_currentStage))
    {
        checks.push_back({"resistorTemp",
                          frame.resistorTemp,
                          m_config.maxResistorTemp * WARN_RATIO,
                          m_config.maxResistorTemp,
                          true,
                          QStringLiteral("Перегрев тормозного резистора")});
    }
    if (isParameterActiveOnStage("dieselPressureMax", m_currentStage))
    {
        checks.push_back({"dieselPressure",
                          frame.dieselPressure,
                          m_config.maxDieselPressure * WARN_RATIO,
                          m_config.maxDieselPressure,
                          true,
                          QStringLiteral("Превышение давления дизеля")});
    }
    if (isParameterActiveOnStage("dieselPressureMin", m_currentStage))
    {
        // для нижней границы порог "warn" — чуть выше, чем alarm, пока заглушкой и просто зарезервируем место для этого, потом решим как правильно, нужно спросить у команды 1
        const double warn = m_config.minDieselPressure + (m_config.minDieselPressure * (1.0 - WARN_RATIO));
        checks.push_back({"dieselPressure",
                          frame.dieselPressure,
                          warn,
                          m_config.minDieselPressure,
                          false,
                          QStringLiteral("Падение давления дизеля")});
    }
    // прогон проверок
    Status worst = Status::OK;
    QString worstMsg;
    QString worstParam;
    for (const auto &c : checks)
    {
        Status s = runCheck(c, frame);
        if (static_cast<int>(s) > static_cast<int>(worst))
        {
            worst = s;
            worstMsg = c.description;
            worstParam = c.parameter;
        }
    }
    // принятие решения
    Decision decision;
    decision.status = worst;
    decision.message = worstMsg;
    if (worst == Status::ALARM)
    {
        m_alarmLatched = true;
        decision.correction = makeStopCommand();
        emit decisionReady(decision);
        // команду останова в модель отправляет другой модуль
        // получив этот сигнал. DataProcessor сам в Modbus не пишит ничего.
        emit controlNeeded(decision.correction,
                           QStringLiteral("ALARM: %1").arg(worstMsg));
        return;
    }
    if (worst == Status::WARNING)
    {
        // если параметр в жёлтой зоне — пробуем сформировать корректировку.
        ActuatorCommand cmd;
        QString reason;
        if (buildCorrection(frame, cmd, reason))
        {
            decision.correction = cmd;
            emit controlNeeded(cmd, reason);
        }
    }
    emit decisionReady(decision);
}
DataProcessor::Status DataProcessor::runCheck(const Check &c, const SensorFrame &frame)
{
    const bool overWarn = c.upperBound ? (c.value >= c.warnThreshold)
                                       : (c.value <= c.warnThreshold);
    const bool overAlarm = c.upperBound ? (c.value >= c.alarmThreshold)
                                        : (c.value <= c.alarmThreshold);
    if (overAlarm)
    {
        AlarmEvent ev;
        ev.runId = frame.runId;
        ev.timestampMs = frame.timestampMs;
        ev.stage = frame.stage;
        ev.parameter = c.parameter;
        ev.currentValue = c.value;
        ev.criticalThreshold = c.alarmThreshold;
        ev.reason = c.description;
        emit alarmRaised(ev);
        return Status::ALARM;
    }
    if (overWarn)
    {
        WarningEvent ev;
        ev.runId = frame.runId;
        ev.timestampMs = frame.timestampMs;
        ev.stage = frame.stage;
        ev.parameter = c.parameter;
        ev.currentValue = c.value;
        ev.threshold = c.warnThreshold;
        ev.description = c.description;
        emit warningRaised(ev);
        return Status::WARNING;
    }
    return Status::OK;
}
bool DataProcessor::isParameterActiveOnStage(const QString &parameter, int stage) const
{
    // исключение ложных срабатываний в неактивных режимах
    if (stage == Stage::IDLE ||
        stage == Stage::COMPLETED ||
        stage == Stage::ABORTED)
    {
        return false;
    }
    // давление дизеля имеет смысл только когда дизель запущен, так же?
    if (parameter == "dieselPressureMin" || parameter == "dieselPressureMax")
    {
        return (stage == Stage::START_AND_WARMUP ||
                stage == Stage::HOT_NO_LOAD ||
                stage == Stage::HOT_WITH_LOAD);
    }
    // температура дизеля смотрим только после запуска.
    if (parameter == "dieselTemp")
    {
        return (stage == Stage::START_AND_WARMUP ||
                stage == Stage::HOT_NO_LOAD ||
                stage == Stage::HOT_WITH_LOAD);
    }
    // температура тормозного резистора только на этапе с нагрузкой.
    if (parameter == "resistorTemp")
    {
        return (stage == Stage::HOT_WITH_LOAD);
    }
    // rpm и motorTemp — на всех активных этапах.
    return true;
}
bool DataProcessor::buildCorrection(const SensorFrame &frame,
                                    ActuatorCommand &outCmd,
                                    QString &outReason) const
{
    // если на этапе с нагрузкой растёт температура
    // тормозного резистора — снижаем тормозной момент, но это надо уточнить у тех кто делает модель
    if (m_currentStage == Stage::HOT_WITH_LOAD &&
        frame.resistorTemp >= m_config.maxResistorTemp * WARN_RATIO)
    {
        outCmd.motorEnabled = true;
        outCmd.targetRpm = frame.rpm;
        outCmd.throttlePosition = m_config.throttleHotLoad;
        outCmd.brakeTorque = m_config.brakeTorqueHotLoad * 0.7; // снизили
        outReason = QStringLiteral(
                        "Снижение тормозного момента: температура резистора %1 близка к пределу")
                        .arg(frame.resistorTemp);
        return true;
    }
    // если обороты близки к лимиту надл прикрыть дроссель.
    const double maxRpm = (m_currentStage == Stage::COLD_CRANKING)
                              ? m_config.maxRpmCold
                              : m_config.maxRpmHot;
    if (frame.rpm >= maxRpm * WARN_RATIO)
    {
        outCmd.motorEnabled = (m_currentStage == Stage::COLD_CRANKING);
        outCmd.targetRpm = m_config.targetRpmCold;
        outCmd.throttlePosition = frame.throttle * 0.8;
        outCmd.brakeTorque = frame.brakeTorque;
        outReason = QStringLiteral(
                        "Снижение дросселя: обороты %1 близки к пределу %2")
                        .arg(frame.rpm)
                        .arg(maxRpm);
        return true;
    }
    return false;
}
ActuatorCommand DataProcessor::makeStopCommand()
{
    ActuatorCommand cmd;
    cmd.motorEnabled = false;
    cmd.targetRpm = 0.0;
    cmd.throttlePosition = 0.0;
    cmd.brakeTorque = 0.0;
    return cmd;
}