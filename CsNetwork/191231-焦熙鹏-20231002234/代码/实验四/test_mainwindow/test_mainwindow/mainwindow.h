


///////////////

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>
#include <QIODevice>
#include <QMessageBox>
#include <QByteArray>
#include <QString>
#include <QTimer>
#include <QComboBox>

#include <QMainWindow>
#include<QTcpServer>
#include<QTcpSocket>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QSerialPort *serial = new QSerialPort();  // 创建串口指针

private slots:
    void on_pushButton_open_clicked();
    void on_pushButton_close_clicked();
    void on_pushButton_output_clicked();
    void on_pushButton_clearoutput_clicked();
    void on_pushButton_clearinput_clicked();
    void on_pushButton_clearack_clicked();
    void on_pushButton_stopoutput_clicked();
    void on_pushButton_pack_clicked();
    void input();
    void sendAck(int ackNumber);
    void checkAckTimeout();
    void updateSlidingWindowDisplay();

    void on_pushButton_clearwin_clicked();

    void on_pushButton_wrong_clicked();

    void on_pushButton_clearparity_clicked();


    void on_pushButton_Client_clicked();


    void on_pushButton_Server_clicked();

    void closeExistingConnections();

    void clientConnected();

    void clientReadData();

    void clientError(QAbstractSocket::SocketError socketError);

    void handleNewConnection();

    void serverReadData();

    void serverDisconnected();

    void processNetworkData(QByteArray data);

private:
    Ui::MainWindow *ui;
    int ackCount = 0;  // ACK计数
    QTimer *ackTimer;  // ACK超时定时器
    int currentSeqNumber = 0;  // 当前发送的序列号
    QByteArray lastSentData;  // 上一次发送的数据
    bool isWaitingForAck = false;  // 是否正在等待ACK
    bool hasShownFirstAckTimeout = false;  // 是否已经显示过第一次ACK超时提示
    QStringList packedData;  // 打包后的数据包列表
    int windowSize = 3;  // 滑动窗口大小
    int currentWindowIndex = 0;  // 当前滑动窗口索引
    static const int MaxPacketSize = 1024;  // 最大数据包大小

    enum class Role { Client, Server };
    Role currentRole = Role::Client;  // 默认角色

    QTcpSocket *tcpSocket = nullptr;  // Client使用
    QTcpServer *tcpServer = nullptr;  // Server使用
};

#endif // MAINWINDOW_H

