#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <QObject>
#include <QTimer>
#include "DataTypes.h"


class StateMachine : public QObject
{
    Q_OBJECT
public:
    explicit StateMachine(ModelConfig& config);
    void requestStart(const ModelConfig &config);
    void requestNextStage();
    void requestAbort(const QString &reason = QString());

    DiagState currentState() const;
    int currentStageIndex() const;

signals:
    void stageChanged(int oldStage, int newStage);
    void finished(DiagState finalState);

private slots:
    void onStageTimeout();

private:
    void transitionTo(DiagState newState);

    DiagState m_state = DiagState::IDLE;
    ModelConfig m_config;
    QTimer *m_stageTimer;
    int m_currentStageIndex = -1;
    int m_previousStageIndex = -1;
};



#endif //State_machine H