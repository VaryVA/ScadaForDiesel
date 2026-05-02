#include "backendcore.h"

BackendCore::BackendCore(QObject *parent)
    : m_backendCommunicator(new BackendCommunicator(this)), QObject{parent}
{

}

void BackendCore::RunCore()
{
    //Запуск таймера опроса модели
}
