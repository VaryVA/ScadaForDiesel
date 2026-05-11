#ifndef TRENDCONTROLLER_H
#define TRENDCONTROLLER_H

#include <QObject>
#include <QTimer>

#include "backendadapter.h"

class TrendController : public QObject
{
    Q_OBJECT

public:
    static TrendController& instance();

    void start();
    void stop();

    bool isRunning() const;

private:
    explicit TrendController(QObject *parent = nullptr);

    BackendAdapter* m_adapter;
    QTimer* m_timer;

    bool m_running = false;
};

#endif // TRENDCONTROLLER_H
