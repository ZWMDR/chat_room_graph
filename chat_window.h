#ifndef CHAT_WINDOW_H
#define CHAT_WINDOW_H

#include <QMainWindow>
#include "ip_info.h"
#include "log_in.h"

#define SIZE 1024

namespace Ui {
class Chat_Window;
}

class Chat_Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Chat_Window(QWidget *parent = nullptr);
    ~Chat_Window();
    void IP_assign(IP_info *IP);
    void recv_hello();

private slots:
    void on_send_bwt_clicked();

    void on_cancel_bwt_clicked();

private:
    Ui::Chat_Window *ui;
    IP_info *IP;
    char *buffer;
    char *buff;

    void socket_recv();
    void clear_buf(char *buf);
};

#endif // CHAT_WINDOW_H
