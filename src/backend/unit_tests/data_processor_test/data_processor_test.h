#include <QtTest>

class DataProcessorTest : public QObject {
    Q_OBJECT
private slots:
    void init(); 
    void cleanup(); 
    void correctFrame();
    void isDecisionReady();
    void isWarningRaised();
    void isAlarmRaised();
    void isControlProduced();
}; // DataProcessorTest
