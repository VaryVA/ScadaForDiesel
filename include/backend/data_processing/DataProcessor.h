#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QObject>
#include <QString>
#include "backend/DataTypes.h"

/**
 * Модуль обработки данных.
 *
 * Принимает SensorFrame от StateMachine, сравнивает с порогами из ModelConfig
 * с учётом текущего этапа и ВОЗВРАЩАЕТ Decision (вектор управляющих воздействий
 * + диагностическое состояние модели + причина).
 * версия 3
 *
 *
 *
 */
class DataProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DataProcessor(const ModelConfig &config, QObject *parent = nullptr);
    Decision processFrame(const SensorFrame &frame);
public slots:
    // уведомление об актуальном этапе (для исключения ложных срабатываний)
    void onStageChanged(int newStage);
    // обновление конфигурации (порогов)
    void onConfigChanged(ModelConfig config);
    // сброс защёлки аварии и внутреннего состояния (новый прогон)
    void reset();

private:
    // активна ли проверка параметра на текущем этапе???
    bool isParamActiveOnStage(const QString &param, int stage) const;
    // аварийная команда останова
    static QVector<ModelControl> makeStopControls();

private:
    ModelConfig m_config;
    int m_currentStage = 0;
    bool m_alarmLatched = false;
};
#endif // DATAPROCESSOR_H
