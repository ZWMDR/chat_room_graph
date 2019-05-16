#ifndef CHAT_WINDOW_H
#define CHAT_WINDOW_H

#include <QMainWindow>
#include <QFile>
#include "ip_info.h"
#include "log_in.h"
#include "emoji.h"

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

private slots:
    void on_send_bwt_clicked();

    void on_cancel_bwt_clicked();

    void on_file_btn_clicked();

    void on_img_btn_clicked();

    void on_emoji_btn_clicked();

private:
    Ui::Chat_Window *ui;
    IP_info *IP;
    char *buffer;
    char *buff;
    bool emoji_flag;
    QString recv_imgs="recv_imgs\\";
    QString recv_files="recv_files\\";
    QString send_imgs="send_imgs\\";

    void socket_recv();
    void clear_buf(char *buf);
};

#endif // CHAT_WINDOW_H
