#ifndef SETTINGS_TAB_H
#define SETTINGS_TAB_H

#include <QWidget>

namespace Ui {
class settings_tab;
}

class settings_tab : public QWidget
{
    Q_OBJECT

public:
    explicit settings_tab(QWidget *parent = nullptr);
    ~settings_tab();

private slots:
    void on_saveSettings_pushButton_clicked();

private:
    Ui::settings_tab *ui;

    void отправки(const QJsonObject &data);
    void отправки2(const QJsonObject &data);

    QJsonObject собратьДанные();
};

#endif // SETTINGS_TAB_H
