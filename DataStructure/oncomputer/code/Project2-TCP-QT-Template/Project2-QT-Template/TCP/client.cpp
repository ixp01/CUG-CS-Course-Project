#include "client.h"
#include "ui_client.h"
#include <QHostAddress>
#include <QDebug>
#include <QTimer>
#include "createrandnums.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

Client::Client(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Client)
{
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    tim = new QTimer(this);
    msgs = new CreateRandNums();
    
    connect(socket, SIGNAL(readyRead()), this, SLOT(recivedata()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(displayError(QAbstractSocket::SocketError)));
    connect(tim, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    
    // 设置定时器间隔为30秒
    tim->setInterval(30000);
    
    // 预设本地测试的IP和端口
    ui->lineEdit_ip->setText("127.0.0.1");
    ui->lineEdit_port->setText("8888");
    
    // 默认不使用哈夫曼编码
    ui->checkBox_huffman->setChecked(false);
    
    // 初始化统计
    totalBytesSent = 0;
    packetsCount = 0;
    
    // 设置底部文本框的初始内容
    updateStatsDisplay();
}

Client::~Client()
{
    delete ui;
    if (msgs != NULL) {
        delete msgs;
    }
}

void Client::displayError(QAbstractSocket::SocketError)
{
    QMessageBox::warning(this, tr("TCP Client"),
                        tr("The following error occurred: %1.")
                        .arg(socket->errorString()));
}

void Client::on_pushButton_connect_clicked()
{
    QString ip = ui->lineEdit_ip->text();
    int port = ui->lineEdit_port->text().toInt();
    
    ui->textEdit_read->append(QString("正在连接到 %1:%2...").arg(ip).arg(port));
    
    socket->connectToHost(ip, port);
    if (socket->waitForConnected(3000)) {
        ui->textEdit_read->append(tr("连接成功！"));
        // 重置统计
        totalBytesSent = 0;
        packetsCount = 0;
        updateStatsDisplay();
        // 连接成功后立即发送第一批数据
        on_pushButton_send_clicked();
    } else {
        ui->textEdit_read->append(QString("连接失败: %1").arg(socket->errorString()));
        updateStatsDisplay();
    }
}

void Client::recivedata()
{
    QString rcvMsg = socket->readAll();
    ui->textEdit_read->append(QString("[server]:%1").arg(rcvMsg));
}

void Client::on_pushButton_send_clicked()
{
    ui->textEdit_read->append("=== 开始新的传输 ===");
    ui->textEdit_read->append("正在生成随机数...");
    msgs->AddRandNums();
    
    bool useHuffman = ui->checkBox_huffman->isChecked();
    ui->textEdit_read->append(QString("编码方式: %1").arg(useHuffman ? "哈夫曼编码" : "普通二进制"));
    
    if (useHuffman) {
        // 使用哈夫曼编码
        msgs->ToHaffmanCode();
    } else {
        // 使用普通二进制编码
        msgs->huffmanDict.clear();
        msgs->huffmanCodes.clear();
        msgs->ToBinaryCode();
    }
    
    msgs->TransformBinaryCode();
    
    QString txt = msgs->strMSG;
    int rawDataSize = SUMNUM * 11; // 原始数据位数
    int compressedSize = txt.length(); // 压缩后字符数
    
    ui->textEdit_read->append(QString("原始数据: %1 位").arg(rawDataSize));
    ui->textEdit_read->append(QString("编码后: %1 字符").arg(compressedSize));
    ui->textEdit_read->append(QString("压缩比: %1:1").arg(rawDataSize / (double)compressedSize, 0, 'f', 2));
    
    if (socket->state() == QAbstractSocket::ConnectedState) {
        QByteArray dataToSend = txt.toUtf8();
        socket->write(dataToSend);
        
        packetsCount++;
        totalBytesSent += dataToSend.size();
        
        ui->textEdit_read->append(QString("[local]:已发送 %1 字节").arg(dataToSend.size()));
        ui->textEdit_read->append(QString("累计: %1 包, %2 字节").arg(packetsCount).arg(totalBytesSent));
        
        // 启动定时器
        if (!tim->isActive()) {
            tim->start();
            ui->textEdit_read->append("定时器已启动，每30秒发送一次");
        }
        
        updateStatsDisplay(); // 更新统计显示
    } else {
        ui->textEdit_read->append("错误：未连接到服务器");
        updateStatsDisplay();
    }
}

void Client::on_pushButton_stop_clicked()
{
    if (tim->isActive()) {
        tim->stop();
        ui->textEdit_read->append("已停止自动发送");
        ui->textEdit_read->append(QString("总计发送: %1 包, %2 字节").arg(packetsCount).arg(totalBytesSent));
        updateStatsDisplay(); // 更新统计显示
    }
}

void Client::onTimeOut()
{
    if (socket->state() != QAbstractSocket::ConnectedState) {
        ui->textEdit_read->append("连接已断开，停止发送");
        tim->stop();
        updateStatsDisplay();
        return;
    }
    
    ui->textEdit_read->append("=== 定时传输 ===");
    msgs->AddRandNums();
    
    bool useHuffman = ui->checkBox_huffman->isChecked();
    
    if (useHuffman) {
        msgs->ToHaffmanCode();
    } else {
        msgs->huffmanDict.clear();
        msgs->huffmanCodes.clear();
        msgs->ToBinaryCode();
    }
    
    msgs->TransformBinaryCode();
    
    QString txt = msgs->strMSG;
    QByteArray dataToSend = txt.toUtf8();
    socket->write(dataToSend);
    
    packetsCount++;
    totalBytesSent += dataToSend.size();
    
    ui->textEdit_read->append(QString("[auto]:已发送 %1 字节 (%2)").arg(dataToSend.size()).arg(useHuffman ? "哈夫曼" : "二进制"));
    updateStatsDisplay(); // 更新统计显示
}

void Client::updateStatsDisplay()
{
    QString stats = "=== 传输统计面板 ===\n";
    stats += QString("连接状态: %1\n").arg(socket->state() == QAbstractSocket::ConnectedState ? "已连接" : "未连接");
    stats += QString("编码方式: %1\n").arg(ui->checkBox_huffman->isChecked() ? "哈夫曼编码" : "普通二进制");
    stats += QString("已发送包数: %1\n").arg(packetsCount);
    stats += QString("总字节数: %1\n").arg(totalBytesSent);
    if (packetsCount > 0) {
        stats += QString("平均包大小: %1 字节\n").arg(totalBytesSent / packetsCount);
    }
    stats += QString("定时器状态: %1\n").arg(tim->isActive() ? "运行中 (30秒间隔)" : "已停止");
    stats += "\n=== 操作说明 ===\n";
    stats += "1. 点击'连接'建立连接\n";
    stats += "2. 勾选复选框切换编码方式\n";
    stats += "3. 点击'发送'开始传输\n";
    stats += "4. 点击'中止'停止自动发送\n";
    stats += "5. 点击'断开'关闭连接";
    
    ui->textEdit_write->setPlainText(stats);
}
