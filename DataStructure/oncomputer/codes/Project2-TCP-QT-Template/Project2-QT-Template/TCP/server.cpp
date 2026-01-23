#include "server.h"
#include "ui_server.h"
#include <QDebug>
#include <QList>
#include <QNetworkInterface>
#include "createrandnums.h"
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);

    init();
	msgs = new CreateRandNums();	//生成处理随机数的类对象
}
//服务器端的析构函数
Widget::~Widget()
{
    delete ui;
    if (msgs != NULL) {
		delete msgs;
	}
}
void Widget::init()
{
    TcpServer = new QTcpServer(this);
    TcpSocket = new QTcpSocket(this);
    newListen();
    connect(TcpServer,SIGNAL(newConnection()),this,SLOT(mconnect()));      //建立连接
    connect(TcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
                this,SLOT(displayError(QAbstractSocket::SocketError)));

}
//检查端口是否被占用
void Widget::newListen()
{
    if(!TcpServer->listen(QHostAddress::Any,8080))
    {
        qDebug()<<TcpServer->errorString();
        close();
        return;
    }
}
//发送数据
void Widget::on_pushButton_send_clicked()
{
    QString str = ui->textEdit_write->toPlainText();
    TcpSocket->write(str.toUtf8().data());
    ui->textEdit_read->append(QString("host:%1").arg(str));
    ui->textEdit_write->clear();
}
//断开连接
void Widget::on_pushButton_close_clicked()
{
    TcpSocket->disconnectFromHost();
    TcpSocket->close();
}

void Widget::mconnect()
{
    qDebug()<<"connect";
    TcpSocket = TcpServer->nextPendingConnection();      //获取当前连接的socket
    connect(TcpSocket,SIGNAL(readyRead()),this,SLOT(RcvData()));
    QString ip = TcpSocket->peerAddress().toString();

    quint16 port = TcpSocket->peerPort();
//    ui->textEdit_read->setText(QString::fromLocal8Bit("[%1:%2]:connected!").arg(ip).arg(port));
    ui->textEdit_read->setText(QString("[%1:%2]:connected").arg(ip).arg(port));
}

void Widget::displayError(QAbstractSocket::SocketError)
{
    qDebug()<<TcpSocket->errorString();
        TcpSocket->close();
}

/*
接收数据，请修改该函数以便实现利用类对象msgs对传输的二进制字符串解码，
请调用msgs->BinaryCodeToIntArray()或者msgs->HaffmanCodeToIntArray()，将二进制字符串解码到msgs->intMSG整形数组中，并基于最大堆或者其他算法动态寻找最大的20个数 / 出现次数最多的20个数
*/
void Widget::RcvData()
{
    QString rcvMsg = TcpSocket->readAll();
//    ui->textEdit_read->setText(rcvMsg);
    QString ip = TcpSocket->peerAddress().toString();
    quint16 port = TcpSocket->peerPort();
    
    //Your Code Here：请修改以下代码，对从客户端接收到的二进制数据进行解码，然后从动态数据中找到最大的20个数 / 出现次数最多的20个数并动态显示
    ui->textEdit_read->append(QString("[%1:%2]:%3").arg(ip).arg(port).arg(rcvMsg));
    
}
