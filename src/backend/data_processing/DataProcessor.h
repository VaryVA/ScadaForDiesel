#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QHash>
#include "DataTypes.h"  //Допишем
// SensorFrame, Decision, WarningEvent, AlarmEvent, ActuatorCommand, Config

/**
 * ver_1_черновик
 * Принимает измерительные пакеты, сверяет значения с
 * критическими порогами и формирует:
 *   - Decision;
 *   - WarningEvent;
 *   - AlarmEvent;
 *   - ActuatorCommand - так и не поняла оставлять это или нет??.
 */
class DataProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DataProcessor(QObject *parent = nullptr);
    // загрузка пороговых значений
    void setConfig(const Config &config);
public slots:
    // приём пакета.
    void processFrame(SensorFrame frame);
    // информирование об этапе испытания
    void onStageChanged(int newStage);
    // сброс внутреннего состояния
    void reset();
signals:
    // итог обработки каждого пакета
    void decisionReady(Decision decision);
    // предупреждение
    void warningRaised(WarningEvent event);
    // авария
    void alarmRaised(AlarmEvent event);
    // коррекция
    void controlNeeded(ActuatorCommand correction, QString reason);

private:
    // описания проверки
    struct Check
    {
        QString parameter;     // имя параметра (для события)
        double value;          // текущее значение
        double warnThreshold;  // порог предупреждения
        double alarmThreshold; // критический порог
        bool upperBound;       // true: нарушение при value > threshold;
                               // false: нарушение при value < threshold
        QString description;
    };
    Status runCheck(const Check &c, const SensorFrame &frame);
    bool isParameterActiveOnStage(const QString &parameter, int stage) const; // допустим ли контроль данного параметра в текущем этапе?
    /// формирование корректирующего воздействия. Возвращает true, если коррекция сформирована.
    bool buildCorrection(const SensorFrame &frame,
                         ActuatorCommand &outCmd,
                         QString &outReason) const;
    // аварийная команда остановки (мотор выкл, дроссель/тормоз = 0).
    static ActuatorCommand makeStopCommand();

private:
    Config m_config;
    int m_currentStage = 0;      // текущий этап
    bool m_alarmLatched = false; // после аварии блокируем дальнейшую генерацию сигналов
    bool m_configured = false;
    // Таймаут отсутствия данных и контроль допустимости управляющих сигналов
    // еще не решили. Поля зарезервированы на всякий случай.
    qint64 m_lastFrameTsMs = 0;
    // qint64 m_dataTimeoutMs = 2000;  // TODO: согласовать
};
#endif // DATAPROCESSOR_H