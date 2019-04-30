#ifndef LOG_IN_H
#define LOG_IN_H

#include <QMainWindow>
#include "ip_info.h"

namespace Ui {
class Log_in;
}

class Log_in : public QMainWindow
{
    Q_OBJECT

public:
    explicit Log_in(QWidget *parent = nullptr);
    ~Log_in();

private slots:
    void on_Log_in_but_clicked();

    void on_cancel_but_clicked();

    void on_sign_in_but_clicked();

private:
    Ui::Log_in *ui;
    IP_info *IP;
};

#endif // LOG_IN_H