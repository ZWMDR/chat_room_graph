#include "chat_window.h"
#include "ui_chat_window.h"
#include <QMessageBox>
#include <QThread>
#include <QtDebug>
#include <regex>
#include <QString>
#include <string>
#include <QFileDialog>
#include <iostream>
#include <string>
#include <sstream>
#include <QDateTime>
#include <QTextDocumentFragment>
#include <QImage>
#include <QThread>

Chat_Window::Chat_Window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Chat_Window)
{
    ui->setupUi(this);
    buff=new char[SIZE];
    buffer=new char[SIZE];
    this->ui->output->setTextColor("red");
    this->ui->output->setFontPointSize(11);

    QDir dir;
    if(!dir.exists(recv_imgs))
        dir.mkdir(recv_imgs);
    if(!dir.exists(recv_files))
        dir.mkdir(recv_files);
    if(!dir.exists(send_imgs))
        dir.mkdir(send_imgs);
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
    if(strncmp(buffer,"SYS_SIGNAL_ONLINE_COUNT:",23)==0)
    {
        std::string st=buffer;
        st=st.substr(24);
        QString num=st.substr(0,3).c_str();
        QString msg=st.substr(4).c_str();
        num="在线人数:"+num+"人";
        this->ui->mesg_output->setPlainText(num);
        msg+="\n\n";
        this->ui->output->append(msg);
    }
    else if(strncmp(buffer,"SYS_SIGNAL_IMG:",14)==0)
    {
        QString cmd=buffer;
        QStringList cmd_split=cmd.split(":");
        qint64 counts=cmd_split[2].toLong();
        std::cout<<"counts="<<counts<<std::endl;
        QString img_name=cmd_split[1];
        QFile img(img_name);
        img.open(QIODevice::WriteOnly);
        std::cout<<"img opened"<<std::endl;
        QObject::disconnect(IP->socket,&QTcpSocket::readyRead,this,&Chat_Window::socket_recv);
        while(counts>0)
        {
            clear_buf(buffer);
            IP->recved=IP->socket->waitForReadyRead(1);
            //QThread::usleep(500);
            qint64 recv_len=this->IP->socket->read(buffer,SIZE);
            img.write(buffer,recv_len);
            counts-=recv_len;
            //std::cout<<"recv:"<<recv_len<<", remaind Bytes:"<<counts<<std::endl;
        }
        img.close();
        QString msg;
        for(int i=3;i<cmd_split.length()-1;i++)
        {
            msg+=cmd_split[i];
            msg+=":";
        }
        this->ui->output->append(msg+"\n");

        QImage image;
        image.load(img_name);
        int w=image.width();
        int h=image.height();
        //std::cout<<"w="<<w<<" h="<<h<<std::endl;
        QTextDocumentFragment fragment;
        if(w>640)
        {
            double scale_rate=640.0/w;
            w=640;
            h*=scale_rate;
        }
        fragment = QTextDocumentFragment::fromHtml("<img src='"+img_name+"'width="+QString::number(w)+" height="+QString::number(h)+" />");
        this->ui->output->textCursor().insertFragment(fragment);
        this->ui->output->setVisible(true);
        this->IP->socket->readAll();
        QObject::connect(IP->socket,&QTcpSocket::readyRead,this,&Chat_Window::socket_recv);
    }
    else if(strncmp(buffer,"SYS_SIGNAL_FILE:",15)==0)
    {
        QString cmd=buffer;
        QStringList cmd_split=cmd.split(":");
        qint64 counts=cmd_split[2].toLong();
        std::cout<<"counts="<<counts<<std::endl;
        QString file_name=cmd_split[1];
        QFile file(file_name);
        file.open(QIODevice::WriteOnly);
        std::cout<<"file opened"<<std::endl;
        QObject::disconnect(IP->socket,&QTcpSocket::readyRead,this,&Chat_Window::socket_recv);
        while(counts>0)
        {
            clear_buf(buffer);
            IP->recved=IP->socket->waitForReadyRead(1);
            qint64 recv_len=this->IP->socket->read(buffer,SIZE);
            file.write(buffer,recv_len);
            counts-=recv_len;
        }
        file.close();
        QString msg;
        for(int i=3;i<cmd_split.length();i++)
        {
            msg+=cmd_split[i];
            if(i<cmd_split.length())
                msg+=":";
        }
        this->ui->output->append(msg+"\n");

        this->IP->socket->readAll();
        QObject::connect(IP->socket,&QTcpSocket::readyRead,this,&Chat_Window::socket_recv);
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

void Chat_Window::on_file_btn_clicked()//文件
{
    QFileDialog *fileDialog=new QFileDialog(this);
    fileDialog->setWindowTitle(tr("选择要发送的文件"));
    fileDialog->setDirectory(".");
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
    }
    for(auto tmp:fileNames)//获取文件完整路径
    {
        qDebug()<<tmp<<endl;
        //获取文件信息
        QFileInfo info(tmp);
        qint64 file_size=info.size();//字节
        std::cout<<"img_size="<<(QString::number((double)file_size/1024,10,2)+"KB,").toStdString()<<std::endl;
        if(file_size/1024/1024>20)//最大不超过20MB
        {
            QMessageBox msg;
            msg.setWindowTitle("提示");
            QString mesg="所选文件大小为:"+QString::number(file_size/1024.0/1024.0,10,2)+"KB,"+"文件太大无法发送!";
            msg.setText(mesg);
            msg.setStyleSheet("font: 8pt;");
            msg.setIcon((QMessageBox::Information));
            msg.addButton(tr("确定"),QMessageBox::ActionRole);
            msg.exec();
            continue;
        }
        qint64 counts=file_size/1024+1;
        //打开文件
        QFile file(tmp);
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox msg;
            msg.setWindowTitle("提示");
            msg.setText("文件打开错误，请稍后重试！");
            msg.setStyleSheet("font: 8pt;");
            msg.setIcon((QMessageBox::Information));
            msg.addButton(tr("确定"),QMessageBox::ActionRole);
            msg.exec();
            continue;
        }
        QDataStream read_in(&file);
        //提取文件名
        QDateTime Time = QDateTime::currentDateTime();//获取系统现在的时间
        QString time = Time.toString("yyyy_MM_dd_hh_mm_ss_ddd");
        QStringList img_name = tmp.split("/");
        QString fl_name=img_name.last();
        //QString fl_dump=send_imgs+time+fl_name;
        //QFile file_dump(fl_dump);
        //file_dump.open(QIODevice::WriteOnly);
        std::cout<<fl_name.toStdString()<<std::endl;
        //发送图片
        clear_buf(buffer);
        strcpy(buffer,"SYS_SIGNAL_FILE:");//系统文件发送信号
        strcat(buffer,(fl_name+":").toStdString().c_str());//图片名
        strcat(buffer,QString::number(file_size,10).toStdString().c_str());//发送字节数
        counts=file_size;
        IP->socket->write(buffer,SIZE);
        IP->socket->waitForBytesWritten(2000);
        while(counts>0)
        {
            clear_buf(buffer);
            file.read(buffer,SIZE);
            qint64 send_len=this->IP->socket->write(buffer,SIZE);
            this->IP->sended=this->IP->socket->waitForBytesWritten(1000);
            //file_dump.write(buffer,SIZE);
            counts-=send_len;
        }
        file.close();
        //file_dump.close();
    }
}

void Chat_Window::on_img_btn_clicked()
{
    QFileDialog *fileDialog=new QFileDialog(this);
    fileDialog->setWindowTitle(tr("选择要发送的图片"));
    fileDialog->setDirectory(".");
    fileDialog->setNameFilter(tr("Images(*.png *.jpg *.jpeg *.bmp)"));
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
    }
    for(auto tmp:fileNames)//获取文件完整路径
    {
        qDebug()<<tmp<<endl;
        //获取文件信息
        QFileInfo info(tmp);
        qint64 img_size=info.size();//字节
        std::cout<<"img_size="<<(QString::number((double)img_size/1024,10,2)+"KB,").toStdString()<<std::endl;
        if(img_size/1024/1024>5)//最大不超过5MB
        {
            QMessageBox msg;
            msg.setWindowTitle("提示");
            QString mesg="所选图片大小为:"+QString::number((double)img_size/1024,10,2)+"KB,"+"图片太大无法发送!";
            msg.setText(mesg);
            msg.setStyleSheet("font: 8pt;");
            msg.setIcon((QMessageBox::Information));
            msg.addButton(tr("确定"),QMessageBox::ActionRole);
            msg.exec();
            continue;
        }
        qint64 counts=img_size/1024+1;
        //打开文件
        QFile file(tmp);
        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox msg;
            msg.setWindowTitle("提示");
            msg.setText("图片打开错误，请稍后重试！");
            msg.setStyleSheet("font: 8pt;");
            msg.setIcon((QMessageBox::Information));
            msg.addButton(tr("确定"),QMessageBox::ActionRole);
            msg.exec();
            continue;
        }
        QDataStream read_in(&file);
        //提取文件名
        QDateTime Time = QDateTime::currentDateTime();//获取系统现在的时间
        QString time = Time.toString("yyyy_MM_dd_hh_mm_ss_ddd");
        QStringList img_name = tmp.split("/");
        QString im_name=img_name.last();
        //QString im_dump=send_imgs+time+im_name;
        //QFile img_dump(im_dump);
        //img_dump.open(QIODevice::WriteOnly);
        std::cout<<im_name.toStdString()<<std::endl;
        //发送图片
        clear_buf(buffer);
        strcpy(buffer,"SYS_SIGNAL_IMG:");//系统图片发送信号
        strcat(buffer,(im_name+":").toStdString().c_str());//图片名
        strcat(buffer,QString::number(img_size,10).toStdString().c_str());//发送字节数
        counts=img_size;
        IP->socket->write(buffer,SIZE);
        IP->socket->waitForBytesWritten(2000);
        while(counts>0)
        {
            clear_buf(buffer);
            file.read(buffer,SIZE);
            qint64 send_len=this->IP->socket->write(buffer,SIZE);
            this->IP->sended=this->IP->socket->waitForBytesWritten(1000);
            //img_dump.write(buffer,SIZE);
            counts-=send_len;
        }
        file.close();
        //img_dump.close();
    }
}
