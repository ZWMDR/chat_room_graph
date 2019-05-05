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
    lst_input_count=0;
    buffer=new char[SIZE];
    if(user_config_exist)
    {
        user_name=user_config->readLine();
        user_pswd=user_config->readLine();

        this->ui->user_name->setText(user_name);
        this->ui->user_pswd->setText(user_pswd);
    }
}

void Log_in::IP_assign(IP_info *IP)
{
    this->IP=IP;
}

Log_in::~Log_in()
{
    delete ui;
}

void Log_in::config_file(QFile *user_config)
{
    this->user_config=user_config;
    user_config_exist=this->user_config->open(QIODevice::ReadOnly | QIODevice::Text);
}

bool Log_in::log_communicate(std::string construction)
{
    QString name=this->ui->user_name->text();
    QString pswd=this->ui->user_pswd->text();
    if(pswd.length()<5 || pswd.length()>18)
    {
        QMessageBox msg;
        msg.setWindowTitle("提示");
        msg.setText("密码长度错误！请输入5-18位密码！");
        msg.setStyleSheet("font: 8pt;");
        msg.setIcon((QMessageBox::Information));
        msg.addButton(tr("确定"),QMessageBox::ActionRole);
        msg.exec();
        return false;
    }

    //打开TCP连接
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
        return false;
    }

    //发送登录信息
    memset(buffer,0,SIZE);
    strcpy(buffer,construction.c_str());
    IP->socket->write(buffer,SIZE);
    IP->sended=IP->socket->waitForBytesWritten(2000);
    if(!IP->sended)
    {
        network_error();
        return false;
    }
    memset(buffer,0,SIZE);
    IP->recved=IP->socket->waitForReadyRead(2000);
    if(!IP->recved)
    {
        network_error();
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
        return false;
    }
    memset(buffer,0,SIZE);
    IP->recved=IP->socket->waitForReadyRead(2000);
    if(!IP->recved)
    {
        network_error();
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
        return false;
    }

    //接收登录信息
    memset(buffer,0,SIZE);
    IP->recved=IP->socket->waitForReadyRead(2000);
    if(!IP->recved)
    {
        network_error();
        return false;
    }
    IP->socket->read(buffer,SIZE);

    cout<<buffer<<endl;
    if(strcmp(buffer,"error1")==0)
    {
        QMessageBox msg;
        msg.setWindowTitle("提示");
        msg.setText("密码错误，请重试！");
        msg.setStyleSheet("font: 8pt;");
        msg.setIcon((QMessageBox::Information));
        msg.addButton(tr("确定"),QMessageBox::ActionRole);
        msg.exec();
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
        return false;
    }
    else
    {
        if(strcmp(buffer,"OK")==0)
        {
            return true;
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
            return true;
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
            return false;
        }
    }
}

void Log_in::on_Log_in_but_clicked()//登录按钮
{
    std::string construction="log_in";
    if(!log_communicate(construction))
    {
        IP->socket->close();
        IP->socket->reset();
        return;
    }
    memset(buffer,0,SIZE);
    strcpy(buffer,"OK");
    IP->socket->write(buffer,SIZE);
    IP->sended=IP->socket->waitForBytesWritten(2000);
    if(!IP->sended)
    {
        network_error();
        IP->socket->close();
        IP->socket->reset();
        return;
    }
    Chat_Window *chat_window=new Chat_Window();
    chat_window->setWindowTitle(" 电子垃圾聊天室");
    chat_window->IP_assign(IP);
    chat_window->show();
    this->close();
}

void Log_in::on_cancel_but_clicked()//取消按钮
{
    this->close();
}

void Log_in::on_sign_in_but_clicked()//注册按钮
{
    std::string construction="sign_in";
    if(!log_communicate(construction))
    {
        IP->socket->close();
        IP->socket->reset();
        return;
    }
    memset(buffer,0,SIZE);
    strcpy(buffer,"OK");
    IP->socket->write(buffer,SIZE);
    IP->sended=IP->socket->waitForBytesWritten(2000);
    if(!IP->sended)
    {
        network_error();
        IP->socket->close();
        IP->socket->reset();
        return;
    }
    Chat_Window *chat_window=new Chat_Window();
    chat_window->setWindowTitle(" 电子垃圾聊天室");
    chat_window->IP_assign(IP);
    chat_window->show();
    this->close();
}

void Log_in::on_user_name_returnPressed()
{
    QString name=this->ui->user_name->text();
    QString pswd=this->ui->user_pswd->text();
    if(name.length()==0)
    {
        QMessageBox msg;
        msg.setWindowTitle("提示");
        msg.setText("请输入账号！");
        msg.setStyleSheet("font: 8pt;");
        msg.setIcon((QMessageBox::Information));
        msg.addButton(tr("确定"),QMessageBox::ActionRole);
        msg.exec();
    }
    else if(pswd.length()==0)
    {
        QMessageBox msg;
        msg.setWindowTitle("提示");
        msg.setText("请输入密码！");
        msg.setStyleSheet("font: 8pt;");
        msg.setIcon((QMessageBox::Information));
        msg.addButton(tr("确定"),QMessageBox::ActionRole);
        msg.exec();
    }
    else {
        on_Log_in_but_clicked();
    }
}

void Log_in::on_user_pswd_returnPressed()
{
    on_user_name_returnPressed();
}
