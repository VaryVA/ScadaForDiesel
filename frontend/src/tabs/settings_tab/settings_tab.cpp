#include "settings_tab.h"
#include "ui_settings_tab.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>

// Технически, тут должны быть функции от бэка, но на руках пока их нет


settings_tab::settings_tab(QWidget *parent) : QWidget(parent), ui(new Ui::settings_tab)
{
    ui->setupUi(this);
}

settings_tab::~settings_tab()
{
    delete ui;
}

QJsonObject settings_tab::собратьДанные()
{
    QJsonObject data;

    // Этапы обкатки
    data["cold"] = ui->lineEdit->text();
    data["hot_no_load"] = ui->lineEdit_2->text();
    data["hot_load"] = ui->lineEdit_3->text();

    // Параметры
    data["rpm"] = ui->lineEdit_7->text();
    data["torque"] = ui->lineEdit_6->text();

    // Уставки
    QJsonObject limits;
    limits["oil_temp_min"] = ui->lineEdit_8->text();
    limits["oil_temp_max"] = ui->lineEdit_10->text();
    limits["oil_press_min"] = ui->lineEdit_9->text();
    limits["oil_press_max"] = ui->lineEdit_11->text();

    data["limits"] = limits;

    return data;
}

void settings_tab::on_saveSettings_pushButton_clicked()
{
    QJsonObject data = собратьДанные();

    qDebug() << "Готовые данные:";
    qDebug() << QJsonDocument(data).toJson();

    // две "отправки"
    отправки(data);
    отправки2(data);
}

void settings_tab::отправки(const QJsonObject &data)
{
    qDebug() << "ОТПРАВКА 1 (этапы + параметры):";
    qDebug() << data["cold"].toString();
    qDebug() << data["rpm"].toString();
}

void settings_tab::отправки2(const QJsonObject &data)
{
    qDebug() << "ОТПРАВКА 2 (уставки):";
    qDebug() << data["limits"];
}

