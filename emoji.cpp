#include "emoji.h"
#include "ui_emoji.h"
#include <iostream>
#include <QTableWidgetItem>
#include <QTabWidget>
#include <QLabel>
#include <QImage>
#include <QPixmap>

Emoji::Emoji(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Emoji)
{
    ui->setupUi(this);
    addEmoji("emotions\\%1.png");
}

Emoji::~Emoji()
{
    delete ui;
}

void Emoji::IP_assign(IP_info *IP)
{
    this->IP=IP;
}

void Emoji::addEmoji(QString path)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);

    for(int i=0;i<8;i++)
        for(int j=0;j<8;j++)
        {
            QTableWidgetItem* tableWidgetItem = new QTableWidgetItem;
            this->ui->tableWidget->setItem(j,i,tableWidgetItem);

            QLabel *emoji_icon=new QLabel();
            emoji_icon->setMargin(2);
            QImage *img=new QImage();
            img->load(path.arg(i+8*j+1));
            QPixmap pixmap = QPixmap::fromImage(*img);
            QPixmap fitpixmap = pixmap.scaled(28, 28, Qt::KeepAspectRatio, Qt::SmoothTransformation);  // 按比例缩放
            emoji_icon->setPixmap(fitpixmap);
            this->ui->tableWidget->setCellWidget(j,i,emoji_icon);
        }
}

void Emoji::on_tableWidget_cellClicked(int row, int column)
{
    std::cout<<"row="<<row<<" column="<<column<<std::endl;
    QString mesg="SYS_SIGNAL_EMOTION:%1.png";
    char buffer[SIZE];
    strcpy(buffer,mesg.arg(column+1+8*row).toStdString().c_str());
    this->IP->socket->write(buffer,SIZE);
    this->IP->sended=this->IP->socket->waitForBytesWritten(2000);
    this->close();
}
