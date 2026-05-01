#include <QtTest>
#include <../src/data_store/data_store.h>

class DataStoreTest : public QObject {
    Q_OBJECT
private slots:
    void init(); 
    void cleanup(); 

    void isFileExist();
    void isWriteRecord();
    void correctWriteRecord();
    void doubleWriteRecord();
    void rewritePrevRecord();
    void isWriteEvent();
    void correctWriteEvent();
    void doubleWriteEvent();
    void rewritePrevEvent();

    void isReadRecord();
    void correctReadRecord();
    void isReadEvent();
    void correctReadEvent();

} // DataStoreTest