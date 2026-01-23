#include "server.h"
#include "ui_server.h"
#include <QMessageBox>
#include <QDebug>
#include <QList>
#include <QNetworkInterface>
#include <QHostAddress>
#include "createrandnums.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    init();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::init()
{
    TcpServer = new QTcpServer(this);
    TcpSocket = new QTcpSocket(this);
    msgs = new CreateRandNums();
    
    // 初始化统计
    totalPacketsReceived = 0;
    totalBytesReceived = 0;
    binaryPackets = 0;
    huffmanPackets = 0;
    totalBinaryBytes = 0;
    totalHuffmanBytes = 0;
    
    // 启动监听
    newListen();
    
    // 连接新连接信号
    connect(TcpServer, SIGNAL(newConnection()), this, SLOT(mconnect()));
    connect(TcpSocket, SIGNAL(readyRead()), this, SLOT(RcvData()));
    connect(TcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    
    // 初始化底部统计面板
    updateServerStatsDisplay();
}

void Widget::newListen()
{
    if (!TcpServer->listen(QHostAddress::Any, 8888))
    {
        QMessageBox::critical(this, tr("TCP Server"),
                             tr("Unable to start the server: %1.")
                             .arg(TcpServer->errorString()));
        close();
        return;
    }
    
    // 显示服务器信息
    ui->textEdit_read->append("=== 服务器信息 ===");
    ui->textEdit_read->append("本地测试IP: 127.0.0.1");
    ui->textEdit_read->append("服务器端口: 8888");
    
    // 显示所有可用的网络接口IP
    ui->textEdit_read->append("\n=== 本机所有IP地址 ===");
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol) {
            QString ipAddress = address.toString();
            if (ipAddress != "127.0.0.1") {
                ui->textEdit_read->append("局域网IP: " + ipAddress);
            }
        }
    }
    
    ui->textEdit_read->append("\n服务器已启动并监听中...");
}

void Widget::updateServerStatsDisplay()
{
    QString stats = "=== 服务器统计面板 ===\n";
    stats += QString("服务器状态: %1\n").arg(TcpServer->isListening() ? "监听中" : "未监听");
    stats += QString("客户端状态: %1\n").arg(TcpSocket->state() == QAbstractSocket::ConnectedState ? "已连接" : "未连接");
    stats += QString("监听端口: 8888\n");
    stats += "\n=== 接收统计 ===\n";
    stats += QString("总包数: %1\n").arg(totalPacketsReceived);
    stats += QString("总字节: %1\n").arg(totalBytesReceived);
    stats += QString("二进制包: %1 (%2字节)\n").arg(binaryPackets).arg(totalBinaryBytes);
    stats += QString("哈夫曼包: %1 (%2字节)\n").arg(huffmanPackets).arg(totalHuffmanBytes);
    
    if (binaryPackets > 0 && huffmanPackets > 0) {
        double avgBinary = totalBinaryBytes / (double)binaryPackets;
        double avgHuffman = totalHuffmanBytes / (double)huffmanPackets;
        double compression = avgBinary / avgHuffman;
        stats += QString("压缩效果: %1%\n").arg((1.0 - 1.0/compression) * 100, 0, 'f', 1);
    }
    
    stats += "\n=== Top-K算法状态 ===\n";
    stats += "✓ 堆算法 (O(n log k))\n";
    stats += "✓ 朴素算法 (O(n log n))\n";
    stats += "✓ 桶排序 (O(n))\n";
    stats += "\n实时处理1万个随机数\n";
    stats += "动态维护频率统计";
    
    ui->textEdit_write->setPlainText(stats);
}

void Widget::mconnect()
{
    TcpSocket = TcpServer->nextPendingConnection();
    connect(TcpSocket, SIGNAL(readyRead()), this, SLOT(RcvData()));
    connect(TcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    ui->textEdit_read->append(tr("Client connected"));
    updateServerStatsDisplay(); // 更新统计显示
}

void Widget::updateTransmissionStats(bool isHuffman, int bytes)
{
    totalPacketsReceived++;
    totalBytesReceived += bytes;
    
    if (isHuffman) {
        huffmanPackets++;
        totalHuffmanBytes += bytes;
    } else {
        binaryPackets++;
        totalBinaryBytes += bytes;
    }
}

void Widget::displayTransmissionStats()
{
    QString stats = "\n=== 传输统计 ===\n";
    stats += QString("总包数: %1, 总字节: %2\n").arg(totalPacketsReceived).arg(totalBytesReceived);
    stats += QString("二进制编码: %1包, %2字节").arg(binaryPackets).arg(totalBinaryBytes);
    if (binaryPackets > 0) {
        stats += QString(" (平均: %1字节/包)").arg(totalBinaryBytes / binaryPackets);
    }
    stats += "\n";
    stats += QString("哈夫曼编码: %1包, %2字节").arg(huffmanPackets).arg(totalHuffmanBytes);
    if (huffmanPackets > 0) {
        stats += QString(" (平均: %1字节/包)").arg(totalHuffmanBytes / huffmanPackets);
    }
    stats += "\n";
    
    if (binaryPackets > 0 && huffmanPackets > 0) {
        double avgBinary = totalBinaryBytes / (double)binaryPackets;
        double avgHuffman = totalHuffmanBytes / (double)huffmanPackets;
        double compression = avgBinary / avgHuffman;
        stats += QString("压缩效果: 哈夫曼编码节省 %1% 的空间\n").arg((1.0 - 1.0/compression) * 100, 0, 'f', 1);
    }
    
    ui->textEdit_read->append(stats);
}

void Widget::RcvData()
{
    QByteArray rcvData = TcpSocket->readAll();
    QString ip = TcpSocket->peerAddress().toString();
    quint16 port = TcpSocket->peerPort();
    
    ui->textEdit_read->append(QString("=== 收到来自 %1:%2 的数据 ===").arg(ip).arg(port));
    ui->textEdit_read->append(QString("数据大小: %1 字节").arg(rcvData.size()));
    
    // 设置接收到的数据
    msgs->strMSG = QString::fromUtf8(rcvData);
    
    // 检查是否是哈夫曼编码
    bool isHuffman = msgs->strMSG.startsWith("HUFFMAN_DICT:");
    
    // 更新传输统计
    updateTransmissionStats(isHuffman, rcvData.size());
    
    if (isHuffman) {
        ui->textEdit_read->append("编码类型: 哈夫曼编码");
        msgs->HaffmanCodeToIntArray();
    } else {
        ui->textEdit_read->append("编码类型: 普通二进制");
        msgs->BinaryCodeToIntArray();
    }
    
    // 准备数据并更新频率表
    std::vector<int> newData(msgs->intMSG, msgs->intMSG + SUMNUM);
    topKTracker.update(newData);
    
    ui->textEdit_read->append("正在计算Top-K...");
    
    // 计算并显示Top-K（三种算法）
    QString result = "\n=== Top-K 算法性能对比 ===\n";
    
    // 堆算法
    algorithmTimer.start();
    auto topKHeap = topKTracker.getTopKWithHeap();
    qint64 heapTime = algorithmTimer.nsecsElapsed();
    
    // 朴素算法
    algorithmTimer.start();
    auto topKNaive = topKTracker.getTopKWithNaive();
    qint64 naiveTime = algorithmTimer.nsecsElapsed();
    
    // 桶排序
    algorithmTimer.start();
    auto topKBucket = topKTracker.getTopKWithBucket();
    qint64 bucketTime = algorithmTimer.nsecsElapsed();
    
    // 显示性能对比
    result += QString("堆算法: %1 μs\n").arg(heapTime/1000.0, 0, 'f', 2);
    result += QString("朴素算法: %1 μs\n").arg(naiveTime/1000.0, 0, 'f', 2);
    result += QString("桶排序: %1 μs\n").arg(bucketTime/1000.0, 0, 'f', 2);
    
    // 显示最快的算法
    qint64 minTime = std::min({heapTime, naiveTime, bucketTime});
    QString fastest;
    if (minTime == heapTime) fastest = "堆算法";
    else if (minTime == naiveTime) fastest = "朴素算法";
    else fastest = "桶排序";
    result += QString("最快算法: %1\n").arg(fastest);
    
    // 显示Top-20结果（只显示前10个）
    result += "\n=== Top-10 热门数字 ===\n";
    for (int i = 0; i < std::min(10, (int)topKHeap.size()); i++) {
        result += QString("%1. 数字 %2: %3次\n").arg(i+1).arg(topKHeap[i].first).arg(topKHeap[i].second);
    }
    
    ui->textEdit_read->append(result);
    
    // 显示传输统计
    displayTransmissionStats();
    
    // 更新底部统计面板
    updateServerStatsDisplay();
}

void Widget::on_pushButton_send_clicked()
{
    QString txt = ui->textEdit_write->toPlainText();
    TcpSocket->write(txt.toUtf8());
    ui->textEdit_read->append(QString("[local]:%1").arg(txt));
}

void Widget::on_pushButton_close_clicked()
{
    TcpSocket->close();
    TcpServer->close();
    ui->textEdit_read->append(tr("Server closed"));
    updateServerStatsDisplay(); // 更新统计显示
}

void Widget::displayError(QAbstractSocket::SocketError)
{
    QMessageBox::warning(this, tr("TCP Server"),
                        tr("The following error occurred: %1.")
                        .arg(TcpSocket->errorString()));
}
