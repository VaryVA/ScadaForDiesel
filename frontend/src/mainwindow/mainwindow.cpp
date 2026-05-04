#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "../tabs/main_tab/main_tab.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Добавляем виджеты на вкладки
    ui->tabWidget->addTab(new main_tab(this), "Мнемосхема");
    ui->tabWidget->setStyleSheet(
        /* Стиль для области содержимого */
        "QTabWidget::pane {"
        "    background-color: #D9D9D9;"
        "    border: none;"
        "}"

        /* Стиль для всех вкладок */
        "QTabBar::tab {"
        "    background-color: #D9D9D9;"
        "    color: #333333;"
        "    border: 1px solid black;"
        "    padding: 8px 16px;"
        "    margin-right: 2px;"
        "}"

        /* Стиль для выбранной (текущей) вкладки */
        "QTabBar::tab:selected {"
        "    background-color: #A0EAB0;"
        "}"

        /* Стиль при наведении (опционально) */
        "QTabBar::tab:hover:!selected {"
        "    background-color: #c8c8c8;"
        "}"
    );

}

MainWindow::~MainWindow()
{
    delete ui;
}

