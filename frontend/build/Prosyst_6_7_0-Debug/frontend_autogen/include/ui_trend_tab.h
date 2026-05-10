/********************************************************************************
** Form generated from reading UI file 'trend_tab.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TREND_TAB_H
#define UI_TREND_TAB_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_trend_tab
{
public:
    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *chartsLayout;
    QChartView *tempChartView;
    QChartView *pressureChartView;
    QChartView *rpmChartView;
    QChartView *torqueChartView;
    QHBoxLayout *topLayout;
    QPushButton *clearButton;

    void setupUi(QWidget *trend_tab)
    {
        if (trend_tab->objectName().isEmpty())
            trend_tab->setObjectName("trend_tab");
        trend_tab->resize(294, 484);
        mainLayout = new QVBoxLayout(trend_tab);
        mainLayout->setObjectName("mainLayout");
        scrollArea = new QScrollArea(trend_tab);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 274, 430));
        chartsLayout = new QVBoxLayout(scrollAreaWidgetContents);
        chartsLayout->setObjectName("chartsLayout");
        tempChartView = new QChartView(scrollAreaWidgetContents);
        tempChartView->setObjectName("tempChartView");

        chartsLayout->addWidget(tempChartView);

        pressureChartView = new QChartView(scrollAreaWidgetContents);
        pressureChartView->setObjectName("pressureChartView");

        chartsLayout->addWidget(pressureChartView);

        rpmChartView = new QChartView(scrollAreaWidgetContents);
        rpmChartView->setObjectName("rpmChartView");

        chartsLayout->addWidget(rpmChartView);

        torqueChartView = new QChartView(scrollAreaWidgetContents);
        torqueChartView->setObjectName("torqueChartView");

        chartsLayout->addWidget(torqueChartView);

        scrollArea->setWidget(scrollAreaWidgetContents);

        mainLayout->addWidget(scrollArea);

        topLayout = new QHBoxLayout();
        topLayout->setObjectName("topLayout");
        clearButton = new QPushButton(trend_tab);
        clearButton->setObjectName("clearButton");

        topLayout->addWidget(clearButton);


        mainLayout->addLayout(topLayout);


        retranslateUi(trend_tab);

        QMetaObject::connectSlotsByName(trend_tab);
    } // setupUi

    void retranslateUi(QWidget *trend_tab)
    {
        clearButton->setText(QCoreApplication::translate("trend_tab", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214", nullptr));
        (void)trend_tab;
    } // retranslateUi

};

namespace Ui {
    class trend_tab: public Ui_trend_tab {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TREND_TAB_H
