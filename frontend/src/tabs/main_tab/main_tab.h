#ifndef MAIN_TAB_H
#define MAIN_TAB_H

#include <QWidget>

namespace Ui {
class main_tab;
}

class main_tab : public QWidget
{
    Q_OBJECT

public:
    explicit main_tab(QWidget *parent = nullptr);
    ~main_tab();

private:
    Ui::main_tab *ui;
};

#endif // MAIN_TAB_H
