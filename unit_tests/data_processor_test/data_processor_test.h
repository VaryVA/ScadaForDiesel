#include <QtTest>
#include <../src/data_processor/data_processor.h>

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

} // DataProcessorTest