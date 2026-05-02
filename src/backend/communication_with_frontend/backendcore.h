#ifndef BACKENDCORE_H
#define BACKENDCORE_H

#include <QObject>
#include "BackendWorker/backendcommunicator.h"

class BackendCore : public QObject
{
    Q_OBJECT
public:
    explicit BackendCore(QObject *parent = nullptr);
    ~BackendCore();

    //Запуск ядра бэкэнда(запуск таймера)
    void RunCore();
    //Остановка ядра
    void StopCore();

    //Геттер для получения указателя на коммуникатор
    BackendCommunicator* GetBackendCommunicator();

private:
    BackendCommunicator* m_backendCommunicator;
};

#endif // BACKENDCORE_H
