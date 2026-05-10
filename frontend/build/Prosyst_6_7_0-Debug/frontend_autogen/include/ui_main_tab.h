/********************************************************************************
** Form generated from reading UI file 'main_tab.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAIN_TAB_H
#define UI_MAIN_TAB_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_main_tab
{
public:
    QFrame *stages_select;
    QRadioButton *hot_no_load;
    QRadioButton *cold;
    QRadioButton *hot_load;
    QLabel *label;
    QFrame *controls;
    QPushButton *stopPushButton;
    QPushButton *startPushButton;
    QFrame *indicator;
    QLabel *label_2;
    QLabel *indicator_color;
    QFrame *values;
    QLabel *back_img;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;

    void setupUi(QWidget *main_tab)
    {
        if (main_tab->objectName().isEmpty())
            main_tab->setObjectName("main_tab");
        main_tab->resize(673, 537);
        main_tab->setStyleSheet(QString::fromUtf8("background-color: #D9D9D9"));
        stages_select = new QFrame(main_tab);
        stages_select->setObjectName("stages_select");
        stages_select->setGeometry(QRect(30, 30, 571, 41));
        stages_select->setStyleSheet(QString::fromUtf8("background-color: #B3AAAA;"));
        stages_select->setFrameShape(QFrame::StyledPanel);
        stages_select->setFrameShadow(QFrame::Raised);
        hot_no_load = new QRadioButton(stages_select);
        hot_no_load->setObjectName("hot_no_load");
        hot_no_load->setGeometry(QRect(230, 10, 161, 23));
        cold = new QRadioButton(stages_select);
        cold->setObjectName("cold");
        cold->setGeometry(QRect(130, 10, 99, 23));
        hot_load = new QRadioButton(stages_select);
        hot_load->setObjectName("hot_load");
        hot_load->setGeometry(QRect(410, 10, 161, 23));
        label = new QLabel(stages_select);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 10, 101, 20));
        controls = new QFrame(main_tab);
        controls->setObjectName("controls");
        controls->setGeometry(QRect(30, 80, 120, 121));
        controls->setStyleSheet(QString::fromUtf8("border: 0px;"));
        controls->setFrameShape(QFrame::StyledPanel);
        controls->setFrameShadow(QFrame::Raised);
        stopPushButton = new QPushButton(controls);
        stopPushButton->setObjectName("stopPushButton");
        stopPushButton->setGeometry(QRect(10, 70, 85, 27));
        stopPushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #A0EAB0;\n"
"    border: 1px solid black;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: #80d890;  /* \320\242\320\265\320\274\320\275\320\265\320\265 \320\275\320\260 20% */\n"
"    border-color: #333333;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: #60c070;  /* \320\225\321\211\320\265 \321\202\320\265\320\274\320\275\320\265\320\265 */\n"
"    border-color: #111111;\n"
"}"));
        startPushButton = new QPushButton(controls);
        startPushButton->setObjectName("startPushButton");
        startPushButton->setGeometry(QRect(10, 10, 85, 27));
        startPushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #A0EAB0;\n"
"    border: 1px solid black;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: #80d890;  /* \320\242\320\265\320\274\320\275\320\265\320\265 \320\275\320\260 20% */\n"
"    border-color: #333333;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: #60c070;  /* \320\225\321\211\320\265 \321\202\320\265\320\274\320\275\320\265\320\265 */\n"
"    border-color: #111111;\n"
"}"));
        indicator = new QFrame(main_tab);
        indicator->setObjectName("indicator");
        indicator->setGeometry(QRect(210, 120, 221, 41));
        indicator->setStyleSheet(QString::fromUtf8("border: 0px"));
        indicator->setFrameShape(QFrame::StyledPanel);
        indicator->setFrameShadow(QFrame::Raised);
        label_2 = new QLabel(indicator);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 10, 141, 19));
        indicator_color = new QLabel(indicator);
        indicator_color->setObjectName("indicator_color");
        indicator_color->setGeometry(QRect(175, 6, 30, 30));
        indicator_color->setStyleSheet(QString::fromUtf8("background-color: #51C763;\n"
"border: 1px solid black;\n"
"border-radius: 15px;"));
        values = new QFrame(main_tab);
        values->setObjectName("values");
        values->setGeometry(QRect(160, 170, 451, 331));
        values->setStyleSheet(QString::fromUtf8("border: 0px"));
        values->setFrameShape(QFrame::StyledPanel);
        values->setFrameShadow(QFrame::Raised);
        back_img = new QLabel(values);
        back_img->setObjectName("back_img");
        back_img->setGeometry(QRect(5, 8, 441, 321));
        back_img->setPixmap(QPixmap(QString::fromUtf8(":/Mnemoschme.png")));
        back_img->setScaledContents(true);
        label_4 = new QLabel(values);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(90, 20, 21, 19));
        label_4->setStyleSheet(QString::fromUtf8("background-color: #736A6A;\n"
"font-weight: 700;\n"
"font-size: 16px"));
        label_4->setAlignment(Qt::AlignCenter);
        label_5 = new QLabel(values);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(210, 40, 21, 19));
        label_5->setStyleSheet(QString::fromUtf8("background-color: #736A6A;\n"
"font-weight: 700;\n"
"font-size: 16px"));
        label_5->setAlignment(Qt::AlignCenter);
        label_6 = new QLabel(values);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(300, 50, 21, 19));
        label_6->setStyleSheet(QString::fromUtf8("background-color: #736A6A;\n"
"font-weight: 700;\n"
"font-size: 16px"));
        label_6->setAlignment(Qt::AlignCenter);
        label_7 = new QLabel(values);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(370, 40, 21, 19));
        label_7->setStyleSheet(QString::fromUtf8("background-color: #736A6A;\n"
"font-weight: 700;\n"
"font-size: 16px"));
        label_7->setAlignment(Qt::AlignCenter);
        label_8 = new QLabel(values);
        label_8->setObjectName("label_8");
        label_8->setGeometry(QRect(90, 170, 21, 21));
        label_8->setStyleSheet(QString::fromUtf8("background-color: #736A6A;\n"
"font-weight: 700;\n"
"font-size: 16px"));
        label_8->setAlignment(Qt::AlignCenter);
        label_9 = new QLabel(values);
        label_9->setObjectName("label_9");
        label_9->setGeometry(QRect(340, 300, 21, 19));
        label_9->setStyleSheet(QString::fromUtf8("background-color: #736A6A;\n"
"font-weight: 700;\n"
"font-size: 16px"));
        label_9->setAlignment(Qt::AlignCenter);

        retranslateUi(main_tab);

        QMetaObject::connectSlotsByName(main_tab);
    } // setupUi

    void retranslateUi(QWidget *main_tab)
    {
        main_tab->setWindowTitle(QCoreApplication::translate("main_tab", "Form", nullptr));
        hot_no_load->setText(QCoreApplication::translate("main_tab", "\320\223\320\276\321\200\321\217\321\207\320\260\321\217 \320\261\320\265\320\267 \320\275\320\260\320\263\321\200\321\203\320\267\320\272\320\270", nullptr));
        cold->setText(QCoreApplication::translate("main_tab", "\320\245\320\276\320\273\320\276\320\264\320\275\320\260\321\217", nullptr));
        hot_load->setText(QCoreApplication::translate("main_tab", "\320\223\320\276\321\200\321\217\321\207\320\260\321\217 \321\201 \320\275\320\260\320\263\321\200\321\203\320\267\320\272\320\276\320\271", nullptr));
        label->setText(QCoreApplication::translate("main_tab", "\320\255\321\202\320\260\320\277\321\213 \320\276\320\261\320\272\320\260\321\202\320\272\320\270:", nullptr));
        stopPushButton->setText(QCoreApplication::translate("main_tab", "\320\241\321\202\320\276\320\277", nullptr));
        startPushButton->setText(QCoreApplication::translate("main_tab", "\320\241\321\202\320\260\321\200\321\202", nullptr));
        label_2->setText(QCoreApplication::translate("main_tab", "\320\230\320\275\320\264\320\270\320\272\320\260\321\202\320\276\321\200 \321\201\320\276\321\201\321\202\320\276\321\217\320\275\320\270\321\217", nullptr));
        indicator_color->setText(QString());
        back_img->setText(QString());
        label_4->setText(QCoreApplication::translate("main_tab", "T", nullptr));
        label_5->setText(QCoreApplication::translate("main_tab", "T1", nullptr));
        label_6->setText(QCoreApplication::translate("main_tab", "M", nullptr));
        label_7->setText(QCoreApplication::translate("main_tab", "F1", nullptr));
        label_8->setText(QCoreApplication::translate("main_tab", "P", nullptr));
        label_9->setText(QCoreApplication::translate("main_tab", "T2", nullptr));
    } // retranslateUi

};

namespace Ui {
    class main_tab: public Ui_main_tab {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_TAB_H
