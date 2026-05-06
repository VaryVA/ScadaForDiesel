#include "main_tab.h"
#include "ui_main_tab.h"

main_tab::main_tab(QWidget *parent) : QWidget(parent), ui(new Ui::main_tab)
{
    ui->setupUi(this);
}

main_tab::~main_tab()
{
    delete ui;
}

void main_tab::on_startPushButton_clicked()
{
    qDebug() << "start";
}

void main_tab::on_stopPushButton_clicked()
{
    qDebug() << "stop";
}
