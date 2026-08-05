#ifndef TREND_TAB_H
#define TREND_TAB_H

#include <QWidget>
#include <QTimer>

#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>

#include "../../core/data/measurementpoint.h"
#include "../../core/data/backendadapter.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class trend_tab;
}
QT_END_NAMESPACE

class trend_tab : public QWidget
{
    Q_OBJECT

public:
    explicit trend_tab(QWidget *parent = nullptr);
    ~trend_tab();

private slots:
    void onNewMeasurement(const MeasurementPoint& point);

    void on_clearButton_clicked();

private:
    Ui::trend_tab *ui;

    QChart* m_tempChart;
    QChart* m_pressureChart;
    QChart* m_rpmChart;
    QChart* m_torqueChart;

    QLineSeries* m_tempSeries;
    QLineSeries* m_pressureSeries;
    QLineSeries* m_rpmSeries;
    QLineSeries* m_torqueSeries;

    QDateTimeAxis* m_tempAxisX;
    QValueAxis* m_tempAxisY;
    QDateTimeAxis* m_pressureAxisX;
    QValueAxis* m_pressureAxisY;
    QDateTimeAxis* m_rpmAxisX;
    QValueAxis* m_rpmAxisY;
    QDateTimeAxis* m_torqueAxisX;
    QValueAxis* m_torqueAxisY;

    void setupChart();
};

#endif // TREND_TAB_H
