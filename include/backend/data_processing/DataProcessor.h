#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QObject>
#include <QString>
#include "backend/DataTypes.h"

/**
 * Модуль обработки данных.
 * версия 4
 * Принимает SensorFrame, сравнивает с порогами из ModelConfig (с учётом этапа),
 * формирует Decision: вектор управляющих воздействий + DiagState + reason.
 *
 *
 *    используем только существующие ControlType
 *     (Throttle, BrakeTorque, TargetRpm, MotorEnable, EmergencyStop).
 *     Вентиляторы охлаждения управляются через MotorEnable/коды режимов
 *     не могут — поэтому при перегревах формируем только DiagState
 *     и текстовую причину; включением вентиляторов будет заниматься тот,
 *     кто реализует applyControls (в Modbus у нас есть CoilsRegisters::fan_*).
 *   - дублируем выдачу Decision двумя путями:
 *       1) processFrame возвращает Decision (если кто-то захочет использовать);
 *       2) сигнал decisionReady(Decision)
 *          закомментированный connect в state_machine.cpp.
 */
class DataProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DataProcessor(const ModelConfig &config, QObject *parent = nullptr);
    Decision processFrame(const SensorFrame &frame);
public slots:
    void onStageChanged(int newStage);
    void onConfigChanged(ModelConfig config);
    void reset();

signals:
    void decisionReady(const Decision &decision);

private:
    bool isParamActiveOnStage(const QString &param, int stage) const;
    static QVector<ModelControl> makeStopControls();

private:
    ModelConfig m_config;
    int m_currentStage = 0;
    bool m_alarmLatched = false;
};

#endif // DATAPROCESSOR_H
