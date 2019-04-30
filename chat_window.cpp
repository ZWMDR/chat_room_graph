#include "chat_window.h"
#include "ui_chat_window.h"
#include <QMessageBox>
#include <QThread>

Chat_Window::Chat_Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Chat_Window)
{
    IP=new IP_info();
    ui->setupUi(this);
}

Chat_Window::~Chat_Window()
{
    IP->socket->close();
    delete ui;
}

void Chat_Window::IP_assign(IP_info *IP)
{
    this->IP=IP;
}
