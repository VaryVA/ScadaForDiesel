#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <QObject>
#include <QTimer>
#include "backend/DataTypes.h"
#include "backend/backend_worker/backendcommunicator.h"
#include "backend/config_data/config_data.h"

class IModbusBridge;
class BackendWorker;

class StateMachine : public QObject
{
    Q_OBJECT
public:
    explicit StateMachine(BackendWorker* backendWorker, QObject* parent = nullptr);
    ~StateMachine();

    void Run();
    void Stop();

    void requestStart(const ModelConfig& config);
    void requestNextStage();
    void requestAbort(const QString& reason = QString());

    DiagState currentState() const;
    int currentStageIndex() const;

signals:
    void stageChanged(int oldStage, int newStage);
    void finished(DiagState finalState);

private slots:
    void onStageTimeout();

    //Слот для обработки пришедшего запроса на изменения этапа эксперимента от фронта
    void onReceivedFrontControl(FrontControl control);
    //Слот для обработки пришедшего кофига от фронта
    void onReceivedModelConfig(ModelConfig config);

private:
    void transitionTo(DiagState newState);

    BackendCommunicator* m_communicator;
    IModbusBridge* m_modbusBridge;

    DiagState m_state = DiagState::IDLE;
    ModelConfig m_config;
    QTimer* m_stageTimer;
    int m_currentStageIndex = -1;
    int m_previousStageIndex = -1;
};
#endif //State_machine H
