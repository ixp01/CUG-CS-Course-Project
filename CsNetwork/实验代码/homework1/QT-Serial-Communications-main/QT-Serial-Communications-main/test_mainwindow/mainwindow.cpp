//////////



#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts())
    {
        ui->comboBox_port->addItem(info.portName());  // 遍历可用串口，将串口写入下拉框中
    }

    QStringList baudList;
    baudList << "115200" << "57600" << "9600" << "4800";
    ui->comboBox_baud->addItems(baudList);  // 将波特率添加到下拉列表
    ui->comboBox_baud->setCurrentIndex(2);  // 设置默认选中波特率9600

    QStringList byteList;
    byteList << "8" << "7" << "6" << "5";
    ui->comboBox_byte->addItems(byteList);  // 将数据位添加到下拉列表
    ui->comboBox_byte->setCurrentIndex(0);  // 设置默认选中数据位8

    QStringList checkList;  // 设置校验位，可选择奇校验或偶校验
    checkList << "None" << "Odd" << "Even";
    ui->comboBox_check->addItems(checkList);

    QStringList stopList;  // 设置停止位，默认为1
    stopList << "1";
    ui->comboBox_stop->addItems(stopList);

    ui->textEdit_input->setReadOnly(true);  // 将接收区设置为只读

    ackTimer = new QTimer(this);  // 创建ACK超时定时器
    connect(ackTimer, &QTimer::timeout, this, &MainWindow::checkAckTimeout);  // 连接定时器超时信号
    isWaitingForAck = false;  // 初始化等待ACK标志
    hasShownFirstAckTimeout = false;  // 初始化第一次ACK超时提示标志
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_open_clicked()//打开串口按钮
{
    serial->setPortName(ui->comboBox_port->currentText());  // 设置所用的串口

    switch (ui->comboBox_baud->currentIndex())  // 设置波特率
    {
    case 0: serial->setBaudRate(QSerialPort::Baud115200); break;
    case 1: serial->setBaudRate(QSerialPort::Baud57600);  break;
    case 2: serial->setBaudRate(QSerialPort::Baud9600);   break;
    case 3: serial->setBaudRate(QSerialPort::Baud4800);   break;
    default: break;
    }

    switch (ui->comboBox_byte->currentIndex())  // 设置数据位
    {
    case 0: serial->setDataBits(QSerialPort::Data8); break;
    case 1: serial->setDataBits(QSerialPort::Data7); break;
    case 2: serial->setDataBits(QSerialPort::Data6); break;
    case 3: serial->setDataBits(QSerialPort::Data5); break;
    default: break;
    }

    serial->setParity(QSerialPort::NoParity);  // 设置校验位为无
    serial->setStopBits(QSerialPort::OneStop);  // 设置停止位为1

    if (serial->open(QIODevice::ReadWrite))
    {
        QMessageBox::about(NULL, "提示", "串口打开成功！");
        connect(serial, &QSerialPort::readyRead, this, &MainWindow::input);  // 连接信号与槽
    }
    else
    {
        QMessageBox::about(NULL, "提示", "串口打开失败\r\n不存在或已被占用");
    }
}

/////
void MainWindow::on_pushButton_output_clicked()//发送信息按钮的函数，设置串口传输数据的最大值
{
    // 发送当前滑动窗口内的数据包
    for (int i = currentWindowIndex; i < qMin(currentWindowIndex + windowSize, packedData.size()); ++i)
    {
        QString packet = packedData.at(i);//挨个发送数据包列表里面的内容
        if(packet.length() > MaxPacketSize)
        {
            QMessageBox::about(NULL, "错误", "数据包大小超过最大限制");
            continue;
        }
        serial->write(packet.toUtf8() + "\n");  // 串口发送数据包，每个数据包后面加上换行符
        //serial->write(packet.toUtf8() + " ");  // 串口发送数据包，每个数据包后面加上换行符
        updateSlidingWindowDisplay();  // 更新滑动窗口显示
    }
    currentWindowIndex += windowSize;  // 移动滑动窗口
    if (currentWindowIndex >= packedData.size())
    {
        currentWindowIndex = 0;  // 重置滑动窗口索引
    }
}

// void MainWindow::on_pushButton_output_clicked() // 发送信息按钮的函数，设置串口传输数据的最大值
// {
//     // 检查是否所有数据包都已发送成功
//     if (currentWindowIndex >= packedData.size())
//     {
//         QMessageBox::about(NULL, "提示", "所有数据包已成功发送！");
//         return;
//     }

//     // 发送当前滑动窗口内的数据包
//     for (int i = currentWindowIndex; i < qMin(currentWindowIndex + windowSize, packedData.size()); ++i)
//     {
//         QString packet = packedData.at(i); // 挨个发送数据包列表里面的内容
//         if (packet.length() > MaxPacketSize)
//         {
//             QMessageBox::about(NULL, "错误", "数据包大小超过最大限制");
//             continue;
//         }
//         serial->write(packet.toUtf8() + "\n"); // 串口发送数据包，每个数据包后面加上换行符
//         updateSlidingWindowDisplay(); // 更新滑动窗口显示
//     }

//     // 启动ACK超时定时器
//     isWaitingForAck = true; // 标记正在等待ACK
//     ackTimer->start(1000); // 启动ACK超时定时器，1秒超时
// }



void MainWindow::on_pushButton_pack_clicked()//打包
{
    QString data = ui->textEdit_output->toPlainText();  // 获取发送区的内容
    packedData.clear();  // 清空打包后的数据包列表
    currentSeqNumber = 1;  // 重置序列号

    // 将数据分割成3个字节的数据包
    for (int i = 0; i < data.length(); i += 3)
    {
        QString packetData = data.mid(i, 3);  // 从位置i开始提取3个字节
        int validBytes = packetData.length();  // 计算有效字节数
        if (packetData.length() < 3)
        {
            packetData += QString("FF").repeated(3 - packetData.length());  // 如果不足3个字节，用'FF'填充
        }
        QString packet = QString::number(currentSeqNumber) + ":" + packetData + ":" + QString::number(validBytes) + ":";  // 构建数据包，格式为 序号:数据:有效字节数:数据校验
        packedData.append(packet);  // 将数据包添加到列表中
        currentSeqNumber++;  // 增加序列号
    }

    // 计算奇偶校验码
    for (QString &packet : packedData)
    {
        int sum = 0;
        int validBytes = packet.section(':', -2, -2).toInt();  // 获取有效字节数
        QString dataPart = packet.section(':', 1, 1).left(validBytes * 2);  // 提取有效数据部分
        for (QChar c : dataPart.toUpper().toUtf8())
        {
            sum += c.toLatin1() - '0';  // 计算校验和
        }
        bool isOdd = sum % 2 == 1;  // 判断校验和是否为奇数
        if (ui->comboBox_check->currentIndex() == 1)  // 奇校验
        {
            packet += isOdd ? "1" : "0";  // 如果是奇校验，校验码为1表示奇数，0表示偶数
        }
        else if (ui->comboBox_check->currentIndex() == 2)  // 偶校验
        {
            packet += isOdd ? "0" : "1";  // 如果是偶校验，校验码为0表示奇数，1表示偶数
        }
    }

    // 显示打包后的数据
    ui->textEdit_pack->clear();  // 清空打包数据显示区
    for (const QString &packet : packedData)
    {
        ui->textEdit_pack->append(packet);  // 将每个数据包添加到打包数据显示区
    }
}




void MainWindow::input()//输入函数，包含了缓冲区的建立，ack和传输信息的读取，奇偶校验的内容
{
    QByteArray buffer = serial->readAll();  // 创建缓冲区，并读取缓冲区的数据
    QString in = ui->textEdit_input->toPlainText();
    in += QString(buffer);
    //ui->textEdit_input->setText(in);  // 更新接收区（追加）

    // 处理接收到的数据包
    QStringList packets = in.split('\n');
    for (const QString &packet : packets)
    {
        if (packet.isEmpty()) continue;
        if (packet.startsWith("ACK:"))  // 如果接收到的是ACK
        {
            int ackNumber = packet.mid(4).toInt();  // 提取ACK编号
            ui->textEdit_ack->append("接收到了ACK" + QString::number(ackNumber));  // 显示ACK
            if (isWaitingForAck && ackNumber == currentSeqNumber)  // 如果正在等待ACK且编号匹配
            {
                isWaitingForAck = false;  // 停止等待ACK
                ackTimer->stop();  // 停止ACK超时定时器
                hasShownFirstAckTimeout = false;  // 重置第一次ACK超时提示标志
                QMessageBox::about(NULL, "提示", "数据传输成功，已收到ACK！");  // 弹出传输成功的窗口
            }
        }
        else  // 如果接收到的是普通数据
        {
            QStringList parts = packet.split(':');
            if (parts.size() < 4) continue;

            int seqNumber = parts[0].toInt();  // 提取序号
            QString dataPart = parts[1];  // 提取数据部分
            int validBytes = parts[2].toInt();  // 提取有效字节数
            QString checksum = parts[3];  // 提取奇偶校验码

            // 提取有效数据
            QString validData = dataPart.left(validBytes * 2);  // 只取有效字节数对应的数据
            validData = validData.replace("FF", "");  // 去除填充字符

            // 在接收区输出有效数据
            ui->textEdit_input->append(validData);

            // 发送ACK
            sendAck(seqNumber);

            // 奇偶校验
            int sum = 0;
            for (QChar c : validData.toUpper().toUtf8())
            {
                sum += c.toLatin1() - '0';  // 计算校验和
            }
            bool isOdd = sum % 2 == 1;  // 判断校验和是否为奇数
            if (ui->comboBox_check->currentIndex() == 1)  // 奇校验
            {
                if (isOdd != (checksum == "1"))
                {
                    ui->textEdit_parity->append("检测到比特错误，奇偶校验不匹配");
                }
                else
                {
                    ui->textEdit_parity->append("奇偶校验匹配");
                }
            }
            else if (ui->comboBox_check->currentIndex() == 2)  // 偶校验
            {
                if (isOdd != (checksum == "0"))
                {
                    ui->textEdit_parity->append("检测到比特错误，奇偶校验不匹配");
                }
                else
                {
                    ui->textEdit_parity->append("奇偶校验匹配");
                }
            }
        }
    }
}




void MainWindow::sendAck(int ackNumber)//发送ACK序号的函数，通过排序码来区分ACK
{
    QByteArray ack = "ACK:" + QString::number(ackNumber).toUtf8();
    //QByteArray ack = "ACK:" + QString::number(ackNumber).toUtf8()+"\n";
    serial->write(ack);  // 发送ACK
}

void MainWindow::checkAckTimeout()//检查是否超时的函数，用来检测是否出现错误
{
    if (isWaitingForAck)
    {
        if (!hasShownFirstAckTimeout)  // 只在第一次超时时弹出窗口
        {
            QMessageBox::about(NULL, "提示", "ACK超时，正在重传数据！");
            hasShownFirstAckTimeout = true;  // 标记已显示第一次ACK超时提示
        }
        ui->textEdit_ack->append("ACK超时，正在重传数据…");  // 在textEdit_ack中显示无法接收到ACK的信息
        serial->write(lastSentData);  // 重传数据
        ackTimer->start(1000);  // 重新启动定时器
    }
}





/////


void MainWindow::updateSlidingWindowDisplay()
{
    // 更新滑动窗口显示
    ui->textEdit_win->clear();
    for (int i = currentWindowIndex; i < qMin(currentWindowIndex + windowSize, packedData.size()); ++i)
    {
        ui->textEdit_win->append(packedData.at(i));
    }
}

void MainWindow::on_pushButton_clearoutput_clicked()
{
    ui->textEdit_output->clear();  // 清空发送区
}

void MainWindow::on_pushButton_clearinput_clicked()
{
    ui->textEdit_input->clear();  // 清空接收区
}

void MainWindow::on_pushButton_close_clicked()
{
    serial->close();
    QMessageBox::about(NULL, "提示", "串口已关闭！");
}

void MainWindow::on_pushButton_clearack_clicked()
{
    ui->textEdit_ack->clear();  // 清空ACK显示
    ackCount = 0;  // 重置ACK计数
    hasShownFirstAckTimeout = false;  // 重置第一次ACK超时提示标志
}

void MainWindow::on_pushButton_stopoutput_clicked()
{
    isWaitingForAck = false;  // 停止等待ACK
    ackTimer->stop();  // 停止ACK超时定时器
    ui->textEdit_ack->append("数据发送已停止");  // 在textEdit_ack中显示停止信息
}



void MainWindow::on_pushButton_clearwin_clicked()
{
    ui->textEdit_win->clear();
}


void MainWindow::on_pushButton_wrong_clicked()
{
    if (currentWindowIndex < packedData.size())
    {
        QString packet = packedData.at(currentWindowIndex);
        // 引入比特错误
        if (packet.length() > 0)
        {
            packet[0] = (packet[0] == '0') ? '1' : '0';  // 翻转第一个比特
        }
        serial->write(packet.toUtf8() + "\n");  // 串口发送带有错误的数据包
        isWaitingForAck = true;  // 标记正在等待ACK
        lastSentData = packet.toUtf8();  // 保存上一次发送的数据
        ackTimer->start(1000);  // 启动ACK超时定时器，1秒超时
    }
}


void MainWindow::on_pushButton_clearparity_clicked()
{
    ui->textEdit_parity->clear();
}




void MainWindow::on_pushButton_Client_clicked()
{

}


void MainWindow::on_pushButton_CS_2_clicked()
{

}

