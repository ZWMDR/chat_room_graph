#include "chat_window.h"
#include "ui_chat_window.h"
#include <QMessageBox>
#include <QThread>

Chat_Window::Chat_Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Chat_Window)
{
    ui->setupUi(this);
    IP=new IP_info();
    buff=new char[SIZE];
    buffer=new char[SIZE];
    connect(IP->socket,SIGNAL(readyRead()),this,SLOT(socket_recv()));
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
}

void Chat_Window::IP_assign(IP_info *IP)
{
    this->IP=IP;
}

void Chat_Window::clear_buf(char *buf)
{
    memset(buf,0,SIZE);
}
