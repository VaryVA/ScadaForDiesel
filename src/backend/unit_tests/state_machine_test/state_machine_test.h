#include <QtTest>

class StateMachineTest : public QObject {
    Q_OBJECT
private slots:
    void init(); 
    void cleanup(); 
    void isStart();
    void moveOnNextStage();
    void isAbort();
    void correctUIStatus();
}; // StateMachineTest
