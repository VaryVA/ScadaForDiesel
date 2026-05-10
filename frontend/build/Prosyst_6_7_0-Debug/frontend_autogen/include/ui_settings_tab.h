/********************************************************************************
** Form generated from reading UI file 'settings_tab.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_TAB_H
#define UI_SETTINGS_TAB_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_settings_tab
{
public:
    QGroupBox *groupBox;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QLabel *label_5;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QLabel *label_2;
    QLineEdit *lineEdit_2;
    QLabel *label_3;
    QLineEdit *lineEdit_3;
    QGroupBox *groupBox_2;
    QLabel *label_6;
    QWidget *layoutWidget1;
    QFormLayout *formLayout_2;
    QLabel *label_9;
    QLineEdit *lineEdit_7;
    QLabel *label_10;
    QLineEdit *lineEdit_6;
    QGroupBox *groupBox_3;
    QLabel *label_7;
    QWidget *layoutWidget2;
    QGridLayout *gridLayout;
    QLabel *label_11;
    QLineEdit *lineEdit_8;
    QLineEdit *lineEdit_10;
    QLabel *label_12;
    QLineEdit *lineEdit_9;
    QLineEdit *lineEdit_11;
    QPushButton *saveSettings_pushButton;

    void setupUi(QWidget *settings_tab)
    {
        if (settings_tab->objectName().isEmpty())
            settings_tab->setObjectName("settings_tab");
        settings_tab->resize(800, 650);
        settings_tab->setMinimumSize(QSize(800, 600));
        settings_tab->setMaximumSize(QSize(16777215, 16777215));
        settings_tab->setStyleSheet(QString::fromUtf8("QWidget {\n"
"    background-color: #D9D9D9;  /* \320\241\320\262\320\265\321\202\320\273\320\276-\321\201\320\265\321\200\321\213\320\271 */\n"
"}"));
        groupBox = new QGroupBox(settings_tab);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(126, 20, 521, 211));
        groupBox->setTabletTracking(false);
        groupBox->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    border: 1px solid black;\n"
"    margin-top: 10px;\n"
"    font: bold 16px;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px 0 5px;\n"
"}"));
        layoutWidget = new QWidget(groupBox);
        layoutWidget->setObjectName("layoutWidget");
        layoutWidget->setGeometry(QRect(20, 30, 451, 158));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(120);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        label_4 = new QLabel(layoutWidget);
        label_4->setObjectName("label_4");
        label_4->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    text-transform: uppercase;\n"
"    color: black;\n"
"}"));

        horizontalLayout_3->addWidget(label_4);

        label_5 = new QLabel(layoutWidget);
        label_5->setObjectName("label_5");
        label_5->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    text-transform: uppercase;\n"
"    color: black;\n"
"}"));

        horizontalLayout_3->addWidget(label_5);


        verticalLayout->addLayout(horizontalLayout_3);

        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        formLayout->setHorizontalSpacing(60);
        formLayout->setVerticalSpacing(20);
        label = new QLabel(layoutWidget);
        label->setObjectName("label");
        QFont font;
        font.setPointSize(10);
        label->setFont(font);

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        lineEdit = new QLineEdit(layoutWidget);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
        lineEdit->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    background-color: #ffffff;\n"
"    color: #000000;\n"
"    border: 1px solid black;\n"
"}"));
        lineEdit->setAlignment(Qt::AlignmentFlag::AlignCenter);
        lineEdit->setDragEnabled(false);

        formLayout->setWidget(0, QFormLayout::FieldRole, lineEdit);

        label_2 = new QLabel(layoutWidget);
        label_2->setObjectName("label_2");
        label_2->setFont(font);

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        lineEdit_2 = new QLineEdit(layoutWidget);
        lineEdit_2->setObjectName("lineEdit_2");
        lineEdit_2->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    background-color: #ffffff;\n"
"    color: #000000;\n"
"    border: 1px solid black;\n"
"}"));
        lineEdit_2->setAlignment(Qt::AlignmentFlag::AlignCenter);

        formLayout->setWidget(1, QFormLayout::FieldRole, lineEdit_2);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName("label_3");
        label_3->setFont(font);

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        lineEdit_3 = new QLineEdit(layoutWidget);
        lineEdit_3->setObjectName("lineEdit_3");
        lineEdit_3->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    background-color: #ffffff;\n"
"    color: #000000;\n"
"    border: 1px solid black;\n"
"}"));
        lineEdit_3->setAlignment(Qt::AlignmentFlag::AlignCenter);

        formLayout->setWidget(2, QFormLayout::FieldRole, lineEdit_3);


        verticalLayout->addLayout(formLayout);

        groupBox_2 = new QGroupBox(settings_tab);
        groupBox_2->setObjectName("groupBox_2");
        groupBox_2->setGeometry(QRect(126, 240, 521, 121));
        groupBox_2->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    border: 1px solid black;\n"
"    margin-top: 10px;\n"
"    font: bold 16px;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px 0 5px;\n"
"}"));
        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName("label_6");
        label_6->setGeometry(QRect(190, 15, 91, 20));
        label_6->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    text-transform: uppercase;\n"
"    color: black;\n"
"}"));
        layoutWidget1 = new QWidget(groupBox_2);
        layoutWidget1->setObjectName("layoutWidget1");
        layoutWidget1->setGeometry(QRect(20, 44, 401, 69));
        formLayout_2 = new QFormLayout(layoutWidget1);
        formLayout_2->setObjectName("formLayout_2");
        formLayout_2->setHorizontalSpacing(85);
        formLayout_2->setContentsMargins(0, 0, 0, 0);
        label_9 = new QLabel(layoutWidget1);
        label_9->setObjectName("label_9");
        label_9->setFont(font);

        formLayout_2->setWidget(0, QFormLayout::LabelRole, label_9);

        lineEdit_7 = new QLineEdit(layoutWidget1);
        lineEdit_7->setObjectName("lineEdit_7");
        lineEdit_7->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    background-color: #ffffff;\n"
"    color: #000000;\n"
"    border: 1px solid black;\n"
"}"));
        lineEdit_7->setAlignment(Qt::AlignmentFlag::AlignCenter);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, lineEdit_7);

        label_10 = new QLabel(layoutWidget1);
        label_10->setObjectName("label_10");
        label_10->setFont(font);

        formLayout_2->setWidget(1, QFormLayout::LabelRole, label_10);

        lineEdit_6 = new QLineEdit(layoutWidget1);
        lineEdit_6->setObjectName("lineEdit_6");
        lineEdit_6->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    background-color: #ffffff;\n"
"    color: #000000;\n"
"    border: 1px solid black;\n"
"}"));
        lineEdit_6->setAlignment(Qt::AlignmentFlag::AlignCenter);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, lineEdit_6);

        groupBox_3 = new QGroupBox(settings_tab);
        groupBox_3->setObjectName("groupBox_3");
        groupBox_3->setGeometry(QRect(126, 370, 523, 121));
        groupBox_3->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    border: 1px solid black;\n"
"    margin-top: 10px;\n"
"    font: bold 16px;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 5px 0 5px;\n"
"}"));
        label_7 = new QLabel(groupBox_3);
        label_7->setObjectName("label_7");
        label_7->setGeometry(QRect(225, 15, 71, 20));
        label_7->setStyleSheet(QString::fromUtf8("QLabel {\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    text-transform: uppercase;\n"
"    color: black;\n"
"}"));
        layoutWidget2 = new QWidget(groupBox_3);
        layoutWidget2->setObjectName("layoutWidget2");
        layoutWidget2->setGeometry(QRect(11, 40, 501, 61));
        gridLayout = new QGridLayout(layoutWidget2);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setHorizontalSpacing(20);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_11 = new QLabel(layoutWidget2);
        label_11->setObjectName("label_11");
        label_11->setFont(font);

        gridLayout->addWidget(label_11, 0, 0, 1, 1);

        lineEdit_8 = new QLineEdit(layoutWidget2);
        lineEdit_8->setObjectName("lineEdit_8");
        lineEdit_8->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    background-color: #ffffff;\n"
"    color: #000000;\n"
"    border: 1px solid black;\n"
"}"));
        lineEdit_8->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout->addWidget(lineEdit_8, 0, 1, 1, 1);

        lineEdit_10 = new QLineEdit(layoutWidget2);
        lineEdit_10->setObjectName("lineEdit_10");
        lineEdit_10->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    background-color: #ffffff;\n"
"    color: #000000;\n"
"    border: 1px solid black;\n"
"}"));
        lineEdit_10->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout->addWidget(lineEdit_10, 0, 2, 1, 1);

        label_12 = new QLabel(layoutWidget2);
        label_12->setObjectName("label_12");
        label_12->setFont(font);

        gridLayout->addWidget(label_12, 1, 0, 1, 1);

        lineEdit_9 = new QLineEdit(layoutWidget2);
        lineEdit_9->setObjectName("lineEdit_9");
        lineEdit_9->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    background-color: #ffffff;\n"
"    color: #000000;\n"
"    border: 1px solid black;\n"
"}"));
        lineEdit_9->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout->addWidget(lineEdit_9, 1, 1, 1, 1);

        lineEdit_11 = new QLineEdit(layoutWidget2);
        lineEdit_11->setObjectName("lineEdit_11");
        lineEdit_11->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    background-color: #ffffff;\n"
"    color: #000000;\n"
"    border: 1px solid black;\n"
"}"));
        lineEdit_11->setAlignment(Qt::AlignmentFlag::AlignCenter);

        gridLayout->addWidget(lineEdit_11, 1, 2, 1, 1);

        saveSettings_pushButton = new QPushButton(settings_tab);
        saveSettings_pushButton->setObjectName("saveSettings_pushButton");
        saveSettings_pushButton->setGeometry(QRect(330, 510, 116, 36));
        saveSettings_pushButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
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

        retranslateUi(settings_tab);

        QMetaObject::connectSlotsByName(settings_tab);
    } // setupUi

    void retranslateUi(QWidget *settings_tab)
    {
        settings_tab->setWindowTitle(QCoreApplication::translate("settings_tab", "Form", nullptr));
        groupBox->setTitle(QString());
        label_4->setText(QCoreApplication::translate("settings_tab", "      \320\255\321\202\320\260\320\277 \320\276\320\261\320\272\320\260\321\202\320\272\320\270", nullptr));
        label_5->setText(QCoreApplication::translate("settings_tab", "    \320\224\320\273\320\270\321\202\320\265\320\273\321\214\320\275\320\276\321\201\321\202\321\214", nullptr));
        label->setText(QCoreApplication::translate("settings_tab", "1. \320\245\320\276\320\273\320\276\320\264\320\275\320\260\321\217:", nullptr));
        lineEdit->setText(QString());
        label_2->setText(QCoreApplication::translate("settings_tab", "2. \320\223\320\276\321\200\321\217\321\207\320\260\321\217 \320\261\320\265\320\267 \320\275\320\260\320\263\321\200\321\203\320\267\320\272\320\270:", nullptr));
        label_3->setText(QCoreApplication::translate("settings_tab", "3. \320\223\320\276\321\200\321\217\321\207\320\260\321\217 \320\277\320\276\320\264 \320\275\320\260\320\263\321\200\321\203\320\267\320\272\320\276\320\271:", nullptr));
        groupBox_2->setTitle(QString());
        label_6->setText(QCoreApplication::translate("settings_tab", "\320\237\320\260\321\200\320\260\320\274\320\265\321\202\321\200\321\213", nullptr));
        label_9->setText(QCoreApplication::translate("settings_tab", "\320\247\320\260\321\201\321\202\320\276\321\202\320\260 \320\262\321\200\320\260\321\211\320\265\320\275\320\270\321\217 \320\255\320\224:", nullptr));
        label_10->setText(QCoreApplication::translate("settings_tab", "\320\242\320\276\321\200\320\274\320\276\320\267\320\275\320\276\320\271 \320\274\320\276\320\274\320\265\320\275\321\202:", nullptr));
        groupBox_3->setTitle(QString());
        label_7->setText(QCoreApplication::translate("settings_tab", " \320\243\321\201\321\202\320\260\320\262\320\272\320\270", nullptr));
        label_11->setText(QCoreApplication::translate("settings_tab", "\320\242\320\265\320\274\320\277\320\265\321\200\320\260\321\202\321\203\321\200\320\260 \320\274\320\260\321\201\320\273\320\260:", nullptr));
        label_12->setText(QCoreApplication::translate("settings_tab", "\320\224\320\260\320\262\320\273\320\265\320\275\320\270\320\265 \320\274\320\260\321\201\320\273\320\260:", nullptr));
        saveSettings_pushButton->setText(QCoreApplication::translate("settings_tab", "\320\241\320\236\320\245\320\240\320\220\320\235\320\230\320\242\320\254", nullptr));
    } // retranslateUi

};

namespace Ui {
    class settings_tab: public Ui_settings_tab {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_TAB_H
