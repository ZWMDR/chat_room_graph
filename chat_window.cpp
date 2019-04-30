#include "chat_window.h"
#include "ui_chat_window.h"
#include <QMessageBox>
#include <QThread>
#include <QtDebug>

Chat_Window::Chat_Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Chat_Window)
{
    ui->setupUi(this);
    buff=new char[SIZE];
    buffer=new char[SIZE];
}

Chat_Window::~Chat_Window()
{
    IP->socket->close();
    delete ui;
}

void Chat_Window::socket_recv()
{
    clear_buf(buffer);
    IP->socket->read(buffer,SIZE);
    this->ui->output->appendPlainText(buffer);
    //std::cout<<buffer<<std::endl;
}

void Chat_Window::IP_assign(IP_info *IP)
{
    this->IP=IP;
    if(!IP->socket->waitForConnected(1000))
        std::cout<<"Network break"<<std::endl;
    else {
        std::cout<<"Network OK"<<std::endl;
        QObject::connect(IP->socket,&QTcpSocket::readyRead,this,&Chat_Window::socket_recv);
    }
}

void Chat_Window::clear_buf(char *buf)
{
    memset(buf,0,SIZE);
}

void Chat_Window::recv_hello()
{
    IP->recved=IP->socket->waitForReadyRead(2000);
    if(IP->recved)
    {
        this->ui->output->appendPlainText(buffer);
        std::cout<<buffer<<std::endl;
    }
}

void Chat_Window::on_send_bwt_clicked()
{
    QString send_msg=this->ui->input->toPlainText();
    clear_buf(buffer);
    strcpy(buffer,send_msg.toStdString().c_str());
    IP->socket->write(buffer,SIZE);
    this->ui->input->clear();
}

void Chat_Window::on_cancel_bwt_clicked()
{
    clear_buf(buffer);
    strcpy(buffer,"SYS_SIGNAL_QUIT");
    IP->socket->write(buffer,SIZE);
    IP->socket->close();
    this->close();
}
