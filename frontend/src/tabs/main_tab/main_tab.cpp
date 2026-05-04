#include "main_tab.h"
#include "ui_main_tab.h"

main_tab::main_tab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::main_tab)
{
    ui->setupUi(this);
}

main_tab::~main_tab()
{
    delete ui;
}
