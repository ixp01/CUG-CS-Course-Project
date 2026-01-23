#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include "createrandnums.h"
namespace Ui {
class Client;
}

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();

private slots:

    void displayError(QAbstractSocket::SocketError);

    void on_pushButton_connect_clicked();

    void recivedata();

    void on_pushButton_send_clicked();

    void on_pushButton_stop_clicked();

public slots:
    void onTimeOut();

private:
    Ui::Client *ui;
    QTcpSocket *socket;
    QTimer *tim;
    CreateRandNums* msgs = NULL;
};

#endif // CLIENT_H
