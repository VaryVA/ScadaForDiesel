#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H
#include <QObject>
#include <QString>
#include "DataTypes.h"
/**
 * Модуль обработки данных.
 *
 * Принимает SensorFrame от StateMachine, сравнивает значения с порогами
 * из ModelConfig (с учётом текущего этапа) и возвращает Decision —
 * вектор управляющих воздействий + диагностическое состояние модели
 * версия 2 - не финальная
 */
class DataProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DataProcessor(const ModelConfig &config, QObject *parent = nullptr);

public slots:
    void processFrame(SensorFrame frame);
    // уведомление об актуальном этапе испытания (для учёта режимов) (согласовать)
    void onStageChanged(int newStage);
    // обновление конфигурации
    void onConfigChanged(ModelConfig config);
    // сброс защёлки аварии и внутреннего состояния (новый прогон)
    void reset();
signals:
    // шотовое решение для StateMachine.
    void decisionReady(Decision decision);

private:
    // активна ли проверка параметра на текущем этапе??? (исключение ложных срабатываний)
    bool isParamActiveOnStage(const QString &param, int stage) const;
    // Сформировать аварийную команду останова
    static QVector<ModelControl> makeStopControls();

private:
    ModelConfig m_config;
    int m_currentStage = 0;
    bool m_alarmLatched = false; // после аварии решения не формируем
};
#endif // DATAPROCESSOR_H