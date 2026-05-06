#pragma once
#include <QObject>
#include <QString>

class ModbusConfig;
class ModelConfig;
class ModelControl;
class SensorFrame;

class IModbusBridge : public QObject
{
    Q_OBJECT
public:
    explicit IModbusBridge(const ModbusConfig& cfg, QObject* parent = nullptr);
    virtual ~IModbusBridge() = default;
public slots:
    // Starting a scheduled data retrieval from the sensor using a timer
    virtual void startPolling() = 0;
    // Stopping a scheduled data retrieval from the sensor
    virtual void stopPolling() = 0;
    // Handling direct call to read sensor data
    virtual void onReadSensors() = 0;
    // Handling direct call to read the model info
    virtual void onReadInfo() = 0;
    // Handling direct call to write new model configuration at startup
    virtual void onWriteConfig(const ModelConfig& cmd) = 0;
    // Handling direct call to write controlling command at runtime
    virtual void onWriteControl(const ModelControl& control) = 0;
signals:
    // A response to the sensor data request has been received
    void sensorsDataReady(const SensorFrame& data);
    // A response to the model info request has been received
    void modelInfoReady(const ModelConfig& data);
    // Error signals
    /**
     * @brief Emitted when configuration fails during startup/setup.
     * Note: config validation should be handled by the config builder,
     * so this error would not even happen.
     */
    void configurationError(const QString& reason);
    /**
     * @brief Emitted when physical connection fails or is lost.
     * Groups QModbusDevice errors: ConnectionError.
     */
    void connectionError(const QString& reason);
    /**
     * @brief Emitted when a read/write request fails.
     * Occurs during: Polling requests, direct read/write requests.
     */
    void requestError(const QString& reason);
    /**
     * @brief Any error not fitting above categories
     */
    void generalError(const QString& reason);
    // Transition states
    void connectionLost();
    void connectionRestored();
};
