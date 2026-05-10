#include "trend_tab.h"
#include "ui_trend_tab.h"

#include "../../core/data/datamanager.h"

trend_tab::trend_tab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::trend_tab)
{
    ui->setupUi(this);

    setupChart();

    connect(&DataManager::instance(),
            &DataManager::newMeasurement,
            this,
            &trend_tab::onNewMeasurement,
            Qt::QueuedConnection);
}

trend_tab::~trend_tab()
{
    delete ui;
}

void trend_tab::setupChart()
{
    // SERIES

    m_tempSeries = new QLineSeries();
    m_pressureSeries = new QLineSeries();
    m_rpmSeries = new QLineSeries();
    m_torqueSeries = new QLineSeries();

    QPen tempPen(QColor(255, 140, 0));
    tempPen.setWidth(2);

    QPen pressurePen(QColor(0, 180, 255));
    pressurePen.setWidth(2);

    QPen rpmPen(QColor(0, 220, 120));
    rpmPen.setWidth(2);

    QPen torquePen(QColor(220, 80, 80));
    torquePen.setWidth(2);

    m_tempSeries->setPen(tempPen);
    m_pressureSeries->setPen(pressurePen);
    m_rpmSeries->setPen(rpmPen);
    m_torqueSeries->setPen(torquePen);

    m_tempSeries->setName("Температура");
    m_pressureSeries->setName("Давление");
    m_rpmSeries->setName("RPM");
    m_torqueSeries->setName("Момент");

    // CHARTS

    m_tempChart = new QChart();
    m_pressureChart = new QChart();
    m_rpmChart = new QChart();
    m_torqueChart = new QChart();

    // ADD SERIES

    m_tempChart->addSeries(m_tempSeries);
    m_pressureChart->addSeries(m_pressureSeries);
    m_rpmChart->addSeries(m_rpmSeries);
    m_torqueChart->addSeries(m_torqueSeries);

    // AXES

    auto createAxisX = []()
    {
        auto axis = new QDateTimeAxis();
        axis->setFormat("hh:mm:ss");
        return axis;
    };

    auto createAxisY = []()
    {
        return new QValueAxis();
    };

    m_tempAxisX = createAxisX();
    m_pressureAxisX = createAxisX();
    m_rpmAxisX = createAxisX();
    m_torqueAxisX = createAxisX();

    m_tempAxisY = createAxisY();
    m_pressureAxisY = createAxisY();
    m_rpmAxisY = createAxisY();
    m_torqueAxisY = createAxisY();

    // RANGES

    m_tempAxisY->setRange(50, 120);
    m_pressureAxisY->setRange(0, 10);
    m_rpmAxisY->setRange(0, 5000);
    m_torqueAxisY->setRange(0, 500);

    // ATTACH

    auto attach = [](QChart* chart,
                     QLineSeries* series,
                     QDateTimeAxis* axisX,
                     QValueAxis* axisY)
    {
        chart->addAxis(axisX, Qt::AlignBottom);
        chart->addAxis(axisY, Qt::AlignLeft);

        series->attachAxis(axisX);
        series->attachAxis(axisY);
    };

    attach(m_tempChart,
           m_tempSeries,
           m_tempAxisX,
           m_tempAxisY);

    attach(m_pressureChart,
           m_pressureSeries,
           m_pressureAxisX,
           m_pressureAxisY);

    attach(m_rpmChart,
           m_rpmSeries,
           m_rpmAxisX,
           m_rpmAxisY);

    attach(m_torqueChart,
           m_torqueSeries,
           m_torqueAxisX,
           m_torqueAxisY);

    // SET CHARTS

    ui->tempChartView->setChart(m_tempChart);
    ui->pressureChartView->setChart(m_pressureChart);
    ui->rpmChartView->setChart(m_rpmChart);
    ui->torqueChartView->setChart(m_torqueChart);

    ui->tempChartView->setMinimumHeight(250);
    ui->pressureChartView->setMinimumHeight(250);
    ui->rpmChartView->setMinimumHeight(250);
    ui->torqueChartView->setMinimumHeight(250);

    ui->tempChartView->setRenderHint(QPainter::Antialiasing);
    ui->pressureChartView->setRenderHint(QPainter::Antialiasing);
    ui->rpmChartView->setRenderHint(QPainter::Antialiasing);
    ui->torqueChartView->setRenderHint(QPainter::Antialiasing);
}

void trend_tab::onNewMeasurement(const MeasurementPoint &point)
{
    qint64 x = point.timestamp.toMSecsSinceEpoch();

    m_tempSeries->append(x, point.oilTemperature);
    m_pressureSeries->append(x, point.oilPressure);
    m_rpmSeries->append(x, point.rpm);
    m_torqueSeries->append(x, point.torque);

    auto updateAxis =
        [&](QDateTimeAxis* axis)
    {
        axis->setRange(
            point.timestamp.addSecs(-60),
            point.timestamp);
    };

    updateAxis(m_tempAxisX);
    updateAxis(m_pressureAxisX);
    updateAxis(m_rpmAxisX);
    updateAxis(m_torqueAxisX);

    const int maxPoints = 500;

    auto trim =
        [&](QLineSeries* series)
    {
        if(series->count() > maxPoints)
            series->remove(0);
    };

    trim(m_tempSeries);
    trim(m_pressureSeries);
    trim(m_rpmSeries);
    trim(m_torqueSeries);
}

void trend_tab::on_clearButton_clicked()
{
    m_tempSeries->clear();
    m_pressureSeries->clear();
    m_rpmSeries->clear();
    m_torqueSeries->clear();
}
