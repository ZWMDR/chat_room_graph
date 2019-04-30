#include "log_in.h"
#include "ui_log_in.h"
#include "chat_window.h"

#define SIZE 1024

using namespace std;

void network_error()
{
    QMessageBox msg;
    msg.setWindowTitle("提示");
    msg.setText("网络错误，请稍后重试！");
    msg.setStyleSheet("font: 8pt;");
    msg.setIcon((QMessageBox::Information));
    msg.exec();
}

Log_in::Log_in(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Log_in)
{
    ui->setupUi(this);
    IP=new IP_info();
    lst_input_count=0;
}

Log_in::~Log_in()
{
    delete ui;
}

bool Log_in::log_communicate(std::string construction)
{
    QString name=this->ui->user_name->text();
    QString pswd=this->ui->user_pswd->text();
    IP->socket->connectToHost(IP->IP_addr.c_str(), IP->IP_port);
    IP->connected=IP->socket->waitForConnected(2000);
    if(!IP->connected)
    {
        QMessageBox msg;
        msg.setWindowTitle("提示");
        msg.setText("连接超时，请稍后重试！");
        msg.setStyleSheet("font: 8pt;");
        msg.setIcon((QMessageBox::Information));
        msg.addButton(tr("确定"),QMessageBox::ActionRole);
        msg.exec();

        IP->socket->close();
        return false;
    }

    //发送登录信息
    char buffer[SIZE];
    strcpy(buffer,construction.c_str());
    IP->socket->write(buffer,SIZE);
    IP->sended=IP->socket->waitForBytesWritten(2000);
    if(!IP->sended)
    {
        network_error();
        IP->socket->close();
        return false;
    }
    memset(buffer,0,SIZE);
    IP->recved=IP->socket->waitForReadyRead(2000);
    if(!IP->recved)
    {
        network_error();
        IP->socket->close();
        return false;
    }
    IP->socket->read(buffer,SIZE);

    memset(buffer,0,SIZE);
    strcpy(buffer,name.toStdString().c_str());
    IP->socket->write(buffer,SIZE);
    IP->sended=IP->socket->waitForBytesWritten(2000);
    if(!IP->sended)
    {
        network_error();
        IP->socket->close();
        return false;
    }
    memset(buffer,0,SIZE);
    IP->recved=IP->socket->waitForReadyRead(2000);
    if(!IP->recved)
    {
        network_error();
        IP->socket->close();
        return false;
    }
    IP->socket->read(buffer,SIZE);


    memset(buffer,0,SIZE);
    strcpy(buffer,pswd.toStdString().c_str());

    IP->socket->write(buffer,SIZE);
    IP->sended=IP->socket->waitForBytesWritten(2000);
    if(!IP->sended)
    {
        network_error();
        IP->socket->close();
        return false;
    }

    //接收登录信息
    memset(buffer,0,SIZE);
    IP->recved=IP->socket->waitForReadyRead(2000);
    if(!IP->recved)
    {
        network_error();
        IP->socket->close();
        return false;
    }
    IP->socket->read(buffer,SIZE);

    if(strcmp(buffer,"error1")==0)
    {
        QMessageBox msg;
        msg.setWindowTitle("提示");
        msg.setText("密码错误，请重试！");
        msg.setStyleSheet("font: 8pt;");
        msg.setIcon((QMessageBox::Information));
        msg.addButton(tr("确定"),QMessageBox::ActionRole);
        msg.exec();

        IP->socket->close();
        return false;
    }
    else if(strcmp(buffer,"error2")==0)
    {
        QMessageBox msg;
        msg.setWindowTitle("提示");
        msg.setText("用户未注册，请先注册账户！");
        msg.setStyleSheet("font: 8pt;");
        msg.setIcon((QMessageBox::Information));
        msg.addButton(tr("确定"),QMessageBox::ActionRole);
        msg.exec();

        IP->socket->close();
        return false;
    }
    else if(strcmp(buffer,"error3")==0)
    {
        QMessageBox msg;
        msg.setWindowTitle("提示");
        msg.setText("用户已登陆，请勿重复登录！");
        msg.setStyleSheet("font: 8pt;");
        msg.setIcon((QMessageBox::Information));
        msg.addButton(tr("确定"),QMessageBox::ActionRole);
        msg.exec();

        IP->socket->close();
        return false;
    }
    else if(strcmp(buffer,"error4")==0)
    {
        QMessageBox msg;
        msg.setWindowTitle("提示");
        msg.setText("账号已存在，请勿重复注册！");
        msg.setStyleSheet("font: 8pt;");
        msg.setIcon((QMessageBox::Information));
        msg.addButton(tr("确定"),QMessageBox::ActionRole);
        msg.exec();

        IP->socket->close();
        return false;
    }
    else if(strcmp(buffer,"error5")==0)
    {
        QMessageBox msg;
        msg.setWindowTitle("提示");
        msg.setText("聊天室人数已达上限，请稍后重试！");
        msg.setStyleSheet("font: 8pt;");
        msg.setIcon((QMessageBox::Information));
        msg.addButton(tr("确定"),QMessageBox::ActionRole);
        msg.exec();

        IP->socket->close();
        return false;
    }
    else
    {
        if(strcmp(buffer,"OK")==0)
        {
            QMessageBox msg;
            msg.setWindowTitle("提示");
            msg.setText("登录成功！");
            msg.setStyleSheet("font: 8pt;");
            msg.setIcon((QMessageBox::Information));
            msg.addButton(tr("确定"),QMessageBox::ActionRole);
            msg.exec();

            Chat_Window *chat_window=new Chat_Window();
            chat_window->setWindowTitle(" 电子垃圾聊天室");
            chat_window->IP_assign(IP);
            chat_window->show();
            this->close();
        }
        else if(strcmp(buffer,"OK1")==0)
        {
            QMessageBox msg;
            msg.setWindowTitle("提示");
            msg.setText("注册成功！已自动登录");
            msg.setStyleSheet("font: 8pt;");
            msg.setIcon((QMessageBox::Information));
            msg.addButton(tr("确定"),QMessageBox::ActionRole);
            msg.exec();

            Chat_Window *chat_window=new Chat_Window();
            chat_window->setWindowTitle(" 电子垃圾聊天室");
            chat_window->IP_assign(IP);
            chat_window->show();
            this->close();
        }
        else
        {
            QMessageBox msg;
            msg.setWindowTitle("提示");
            msg.setText("未知网络错误，请稍后重试！");
            msg.setStyleSheet("font: 8pt;");
            msg.setIcon((QMessageBox::Information));
            msg.addButton(tr("确定"),QMessageBox::ActionRole);
            msg.exec();

            IP->socket->close();
            return false;
        }
    }
    return true;
}

void Log_in::on_Log_in_but_clicked()//登录按钮
{
    std::string construction="log_in";
    log_communicate(construction);
    IP->socket->close();
}

void Log_in::on_cancel_but_clicked()//取消按钮
{
    this->close();
}

void Log_in::on_sign_in_but_clicked()//注册按钮
{
    std::string construction="sign_in";
    log_communicate(construction);
    IP->socket->close();
}
