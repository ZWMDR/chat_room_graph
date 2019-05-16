#ifndef EMOJI_H
#define EMOJI_H

#include <QMainWindow>
#include <chat_window.h>
#include <ip_info.h>

namespace Ui {
class Emoji;
}

class Emoji : public QMainWindow
{
    Q_OBJECT

public:
    explicit Emoji(QWidget *parent = nullptr);
    ~Emoji();
    void IP_assign(IP_info *IP);

private slots:
    void on_tableWidget_cellClicked(int row, int column);

private:
    Ui::Emoji *ui;
    void addEmoji(QString path);
    IP_info *IP;
};

#endif // EMOJI_H
