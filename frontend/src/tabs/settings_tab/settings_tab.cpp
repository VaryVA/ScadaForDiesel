#include "settings_tab.h"
#include "ui_settings_tab.h"

settings_tab::settings_tab(QWidget *parent) : QWidget(parent), ui(new Ui::settings_tab)
{
    ui->setupUi(this);
}

settings_tab::~settings_tab()
{
    delete ui;
}

void settings_tab::on_saveSettings_pushButton_clicked()
{
    qDebug() << "Settings tab: clicked";
}
