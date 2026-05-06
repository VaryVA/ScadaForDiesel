#include <QtTest>

class ModbusDataBridgeTest : public QObject {
    Q_OBJECT
private slots:
    void init(); 
    void cleanup(); 
    void isStartPolling();
    void doubleStartPolling();
    void isStopPolling();
    void doubleStopPolling();
    void isWriteCommand();
    void correctWriteCommand();
    void isReadData();
    void correctReadData();
    void correctReadError();
}; // ModbusDataBridgeTest
