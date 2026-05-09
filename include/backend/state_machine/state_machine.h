#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <QObject>
#include <QTimer>

#include "backend/DataTypes.h"
#include "backend/config_data/config_data.h"

#include "backend/backend_worker/backendcommunicator.h"
#include "backend/modbus_client/IModbusBridge.h"
#include "backend/data_processing/DataProcessor.h"

class BackendWorker;
class IModbusBridge;
class DataStore;
class DataProcessor;
class FileConnector;

class StateMachine : public QObject
{
    Q_OBJECT
public:
    explicit StateMachine(BackendWorker* backendWorker, QObject* parent = nullptr);
    ~StateMachine();

    void start();
    void stop();

    void requestNextStage();
    void requestAbort(const QString& reason = QString());

    DiagState currentState() const;
    int currentStageIndex() const;

signals:
    void stageChanged(int oldStage, int newStage);
    void finished(DiagState finalState);

private slots:
    void pollModbus();
    void onSensorsDataReady(const SensorFrame& frame);
    void onDecisionReady(const Decision& decision);

    // Ошибки Modbus
    void onModbusConfigurationError(const QString& reason);
    void onModbusConnectionError(const QString& reason);
    void onModbusRequestError(const QString& reason);
    void onModbusConnectionLost();
    void onModbusConnectionRestored();

    // Команды фронта
    void onReceivedFrontControl(const ModelControl& control);
    void onReceivedModelConfig(const ModelConfig& config);

private:
    void transitionTo(DiagState newState);
    void applyControls(const QVector<ModelControl>& controls);

    // Коммуникатор
    BackendCommunicator* m_communicator;

    // Модули
    IModbusBridge*   m_modbusBridge;
    DataStore*       m_dataStore;
    DataProcessor*   m_dataProcessor;

    //Коннектор
    FileConnector* m_Connector;

    // Таймеры
    QTimer* m_pollTimer;
    QTimer* m_stageTimer;

    // Состояние
    DiagState m_state = DiagState::IDLE;
    ModelConfig m_config;
    SensorFrame m_currentSensorFrame;
    int m_currentStageIndex = -1;
    int m_currentRunId = 0;        // для связывания записей в datastore
    qint64 m_runStartTime = 0;     // для репортов
};

#endif // STATE_MACHINE_H