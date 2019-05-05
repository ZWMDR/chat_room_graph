#ifndef IP_INFO_H
#define IP_INFO_H
#include <QTcpSocket>
#include <string>
#include <iostream>

class IP_info
{
public:
    IP_info(unsigned short port)
    {
        IP_addr="122.152.205.193";
        IP_port=port;
        connected=false;
        sended=false;
        recved=false;
        socket=new QTcpSocket();
    }
    IP_info()
    {
        IP_addr="122.152.205.193";
        IP_port=1778;
        connected=false;
        sended=false;
        recved=false;
        socket=new QTcpSocket();
    }

    std::string IP_addr;
    unsigned short IP_port;
    bool connected;
    bool sended;
    bool recved;
    QTcpSocket *socket;
};
#endif // IP_INFO_H
