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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_trend_tab
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *controlsLayout;
    QCheckBox *tempCheckBox;
    QCheckBox *pressureCheckBox;
    QCheckBox *rpmCheckBox;
    QCheckBox *torqueCheckBox;
    QPushButton *clearButton;
    QChartView *chartView;

    void setupUi(QWidget *trend_tab)
    {
        if (trend_tab->objectName().isEmpty())
            trend_tab->setObjectName("trend_tab");
        verticalLayout = new QVBoxLayout(trend_tab);
        verticalLayout->setObjectName("verticalLayout");
        controlsLayout = new QHBoxLayout();
        controlsLayout->setObjectName("controlsLayout");
        tempCheckBox = new QCheckBox(trend_tab);
        tempCheckBox->setObjectName("tempCheckBox");
        tempCheckBox->setChecked(true);

        controlsLayout->addWidget(tempCheckBox);

        pressureCheckBox = new QCheckBox(trend_tab);
        pressureCheckBox->setObjectName("pressureCheckBox");
        pressureCheckBox->setChecked(true);

        controlsLayout->addWidget(pressureCheckBox);

        rpmCheckBox = new QCheckBox(trend_tab);
        rpmCheckBox->setObjectName("rpmCheckBox");
        rpmCheckBox->setChecked(true);

        controlsLayout->addWidget(rpmCheckBox);

        torqueCheckBox = new QCheckBox(trend_tab);
        torqueCheckBox->setObjectName("torqueCheckBox");
        torqueCheckBox->setChecked(true);

        controlsLayout->addWidget(torqueCheckBox);

        clearButton = new QPushButton(trend_tab);
        clearButton->setObjectName("clearButton");

        controlsLayout->addWidget(clearButton);


        verticalLayout->addLayout(controlsLayout);

        chartView = new QChartView(trend_tab);
        chartView->setObjectName("chartView");

        verticalLayout->addWidget(chartView);


        retranslateUi(trend_tab);

        QMetaObject::connectSlotsByName(trend_tab);
    } // setupUi

    void retranslateUi(QWidget *trend_tab)
    {
        tempCheckBox->setText(QCoreApplication::translate("trend_tab", "\320\242\320\265\320\274\320\277\320\265\321\200\320\260\321\202\321\203\321\200\320\260", nullptr));
        pressureCheckBox->setText(QCoreApplication::translate("trend_tab", "\320\224\320\260\320\262\320\273\320\265\320\275\320\270\320\265", nullptr));
        rpmCheckBox->setText(QCoreApplication::translate("trend_tab", "RPM", nullptr));
        torqueCheckBox->setText(QCoreApplication::translate("trend_tab", "\320\234\320\276\320\274\320\265\320\275\321\202", nullptr));
        clearButton->setText(QCoreApplication::translate("trend_tab", "\320\236\321\207\320\270\321\201\321\202\320\270\321\202\321\214", nullptr));
        (void)trend_tab;
    } // retranslateUi

};

namespace Ui {
    class trend_tab: public Ui_trend_tab {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TREND_TAB_H
