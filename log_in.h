#ifndef LOG_IN_H
#define LOG_IN_H

#include <QMainWindow>
#include <QTcpSocket>
#include <string>
#include <QMessageBox>
#include <iostream>
#include <QDebug>
#include <string.h>
#include "ip_info.h"
#include <fstream>

namespace Ui {
class Log_in;
}

class Log_in : public QMainWindow
{
    Q_OBJECT

public:
    explicit Log_in(QWidget *parent = nullptr);
    void IP_assign(IP_info *IP);
    ~Log_in();

private slots:
    void on_Log_in_but_clicked();

    void on_cancel_but_clicked();

    void on_sign_in_but_clicked();

    void on_user_name_returnPressed();

    void on_user_pswd_returnPressed();

private:
    Ui::Log_in *ui;
    IP_info *IP;
    int lst_input_count;
    char *buffer;
    bool user_config_exist;
    QString user_name;
    QString user_pswd;
    bool _config_;

    bool log_communicate(std::string);
    bool file_open();
    bool file_write(QString name,QString pswd);
};

#endif // LOG_IN_H
