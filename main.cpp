#include "log_in.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IP_info *IP=new IP_info();
    QFile *user_config=new QFile("user_info.ini");
    Log_in log_in;
    log_in.IP_assign(IP);
    log_in.config_file(user_config);
    log_in.setWindowTitle(" 电子垃圾聊天室");
    log_in.show();

    return a.exec();
}
