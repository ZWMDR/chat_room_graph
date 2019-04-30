#include "log_in.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IP_info *IP=new IP_info();
    Log_in log_in;
    log_in.IP_assign(IP);
    log_in.setWindowTitle(" 电子垃圾聊天室");
    log_in.show();

    return a.exec();
}
