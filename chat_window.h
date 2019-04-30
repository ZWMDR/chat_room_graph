#ifndef CHAT_WINDOW_H
#define CHAT_WINDOW_H

#include <QMainWindow>
#include "ip_info.h"

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

private:
    Ui::Chat_Window *ui;
    IP_info *IP;
};

#endif // CHAT_WINDOW_H
