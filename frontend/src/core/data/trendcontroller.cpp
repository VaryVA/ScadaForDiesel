#include "trendcontroller.h"

TrendController::TrendController(QObject *parent)
    : QObject(parent)
{
    m_adapter = new BackendAdapter(this);

    m_timer = new QTimer(this);

    connect(m_timer,
            &QTimer::timeout,
            m_adapter,
            &BackendAdapter::generateMockData);
}

TrendController& TrendController::instance()
{
    static TrendController instance;
    return instance;
}

void TrendController::start()
{
    if(m_running)
        return;

    m_timer->start(500);

    m_running = true;
}

void TrendController::stop()
{
    if(!m_running)
        return;

    m_timer->stop();

    m_running = false;
}

bool TrendController::isRunning() const
{
    return m_running;
}
