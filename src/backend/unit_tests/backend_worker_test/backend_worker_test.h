#include <QtTest>

class BackendWorkerTest : public QObject {
    Q_OBJECT
private slots:
    void init(); 
    void cleanup(); 

    void isStart();
    void doubleStart();
    void isStop();
    void doubleStop();
    void moveOnNextStage();
    void correctCurrentStatus();
    void isFinishedReport();
    void isFinishedHistory();
    void isAlarmEvent();
    void isWarningEvent();
    void isProcessFinished();
}; // BackendWorkerTest
