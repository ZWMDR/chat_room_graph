#include "chat_window.h"
#include "ui_chat_window.h"
#include <QMessageBox>
#include <QThread>
#include <QtDebug>
#include <regex>
#include <string>

Chat_Window::Chat_Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Chat_Window)
{
    ui->setupUi(this);
    buff=new char[SIZE];
    buffer=new char[SIZE];
    this->ui->output->setTextColor("red");
    this->ui->output->setFontPointSize(11);
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
    //this->ui->output->appendPlainText(buffer);
    if(strncmp(buffer,"SYS_SIGNAL_ONLINE_COUNT:",23)==0)
    {
        std::string st=buffer;
        st=st.substr(24);
        QString num=st.substr(0,3).c_str();
        QString msg=st.substr(4).c_str();
        num="在线人数:"+num+"人";
        this->ui->mesg_output->appendPlainText(num);
        msg+="\n\n";
        this->ui->output->append(msg);
    }
    else
    {
        strcat(buffer,"\n");
        this->ui->output->append(buffer);
    }
}

void Chat_Window::IP_assign(IP_info *IP)
{
    this->IP=IP;
    if(!IP->socket->waitForConnected(2000))
        std::cout<<"Network break"<<std::endl;
    else
    {
        std::cout<<"Network OK"<<std::endl;
        QObject::connect(IP->socket,&QTcpSocket::readyRead,this,&Chat_Window::socket_recv);
        std::cout<<"connected"<<std::endl;
    }
}

void Chat_Window::clear_buf(char *buf)
{
    memset(buf,0,SIZE);
}

void Chat_Window::on_send_bwt_clicked()
{
    QString send_msg=this->ui->input->toPlainText();
    clear_buf(buffer);
    strcpy(buffer,send_msg.toStdString().c_str());
    IP->socket->write(buffer,SIZE);
    IP->sended=IP->socket->waitForBytesWritten(2000);
    this->ui->input->clear();
}

void Chat_Window::on_cancel_bwt_clicked()
{
    clear_buf(buffer);
    strcpy(buffer,"SYS_SIGNAL_QUIT");
    IP->socket->write(buffer,SIZE);
    IP->socket->waitForBytesWritten(2000);
    IP->socket->close();
    IP->socket->reset();
    this->close();
}
