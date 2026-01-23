#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include "createrandnums.h"
namespace Ui {
class Server;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void init();

    void newListen();

    void mconnect();

    void RcvData();

    void on_pushButton_send_clicked();

    void on_pushButton_close_clicked();

    void displayError(QAbstractSocket::SocketError);

private:
    Ui::Server *ui;
    QTcpServer *TcpServer;
    QTcpSocket *TcpSocket;
    CreateRandNums* msgs = NULL;
};

#endif // WIDGET_H
