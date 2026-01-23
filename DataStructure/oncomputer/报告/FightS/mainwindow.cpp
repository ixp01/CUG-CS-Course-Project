#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QInputDialog>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMenuBar>
#include <QAction>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QCloseEvent>
#include <QDebug>
#include <exception>
#include <stdexcept>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // 初始化管理器
    flightManager = new FlightManager(this);
    ticketManager = new TicketManager(flightManager, this);
    routeManager = new RouteGraphManager(flightManager, this);
    notificationManager = new NotificationManager(flightManager, routeManager, this);
    
    // 设置文件路径
    flightManager->setDataFilePath("flights.txt");
    ticketManager->setDataFilePath("tickets.txt");
    
    // 加载航班数据
    QString flightsPath = "flights.txt";
    if (!QFile::exists(flightsPath)) {
        QMessageBox::warning(this, "警告", "航班数据文件不存在！\n请确保 flights.txt 文件在正确位置。");
    } else if (!flightManager->loadFromFile(flightsPath)) {
        QMessageBox::warning(this, "警告", "航班数据文件格式错误！\n请确保文件格式正确。");
    } else {
        QMessageBox::information(this, "成功", "航班数据加载成功！");
    }
    
    // 加载票务数据
    QString ticketsPath = "tickets.txt";
    if (!ticketManager->loadFromFile(ticketsPath)) {
        qDebug() << "票务数据加载失败，文件可能不存在";
    } else {
        int loadedCount = ticketManager->getAllTickets().size();
        qDebug() << "票务数据加载成功，共加载" << loadedCount << "张票";
        if (loadedCount > 0) {
            QMessageBox::information(this, "成功", QString("票务数据加载成功！共加载 %1 张票").arg(loadedCount));
        }
    }
    
    // 启动文件监控
    flightManager->startFileMonitoring();
    ticketManager->startFileMonitoring();
    
    // 连接信号槽
    connectSignals();
    
    // 初始化UI
    initializeUI();
    
    // 加载初始数据
    updateFlightTable();
    updateTicketTable();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    // 设置窗口标题和大小
    setWindowTitle("航班管理系统");
    resize(1200, 800);
    
    // 设置表格属性
    setupTables();
    
    // 设置样式
    setupStyles();
    
    // 连接信号和槽
    connectSignals();
}

void MainWindow::setupTables()
{
    // 设置航班表格
    ui->flightTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->flightTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->flightTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // 设置结果表格
    ui->resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->resultTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->resultTable->setVisible(false);
}

void MainWindow::setupStyles()
{
    setStyleSheet(R"(
        QPushButton {
            padding: 8px 16px;
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 4px;
            min-width: 100px;
        }
        QPushButton:hover { background-color: #45a049; }
        QPushButton:pressed { background-color: #3d8b40; }
        QTableWidget {
            border: 1px solid #ddd;
            border-radius: 4px;
            background-color: white;
        }
        QTableWidget::item { padding: 8px; }
        QHeaderView::section {
            background-color: #f5f5f5;
            padding: 8px;
            border: 1px solid #ddd;
            font-weight: bold;
        }
    )");
}

void MainWindow::setTableRow(QTableWidget* table, int row, const Flight* flight)
{
    table->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(flight->flightID)));
    table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(flight->airline)));
    table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(flight->fromCity)));
    table->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(flight->toCity)));
    table->setItem(row, 4, new QTableWidgetItem(flight->departureTime.toString("yyyy-MM-dd hh:mm")));
    table->setItem(row, 5, new QTableWidgetItem(flight->arrivalTime.toString("yyyy-MM-dd hh:mm")));
    table->setItem(row, 6, new QTableWidgetItem(QString::number(flight->price, 'f', 2)));
    table->setItem(row, 7, new QTableWidgetItem(QString::number(flight->totalSeats)));
    table->setItem(row, 8, new QTableWidgetItem(QString::number(flight->availableSeats)));
    
    // 根据余票情况设置颜色
    QTableWidgetItem* availableSeatsItem = table->item(row, 8);
    if (flight->availableSeats == 0) {
        availableSeatsItem->setBackground(QColor(255, 200, 200)); // 浅红色，无余票
    } else if (flight->availableSeats <= 5) {
        availableSeatsItem->setBackground(QColor(255, 255, 200)); // 浅黄色，余票紧张
    } else {
        availableSeatsItem->setBackground(QColor(200, 255, 200)); // 浅绿色，余票充足
    }
    
    // 状态列
    QString statusText;
    switch (flight->status) {
        case FlightStatus::Normal:
            statusText = "正常";
            break;
        case FlightStatus::Delayed:
            statusText = "延误";
            break;
        case FlightStatus::Cancelled:
            statusText = "取消";
            break;
    }
    table->setItem(row, 9, new QTableWidgetItem(statusText));
}

void MainWindow::updateFlightTable()
{
    updateTable(ui->flightTable, flightManager->getAllFlights());
}

void MainWindow::updateResultTable(const QList<Flight*>& flights)
{
    ui->resultTable->setVisible(true);
    updateTable(ui->resultTable, flights);
}

void MainWindow::showMessage(const QString& title, const QString& message, QMessageBox::Icon icon)
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.setIcon(icon);
    msgBox.exec();
}

void MainWindow::showAddFlightDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("添加航班");
    
    QFormLayout* layout = new QFormLayout(&dialog);
    auto inputs = createFlightInputs(&dialog);
    addInputsToLayout(layout, inputs);
    
    QDialogButtonBox* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, &dialog);
    layout->addRow(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        Flight flight = createFlightFromInputs(inputs);
        flightManager->addFlight(flight);
    }
}

QMap<QString, QWidget*> MainWindow::createFlightInputs(QWidget* parent)
{
    QMap<QString, QWidget*> inputs;
    inputs["flightID"] = new QLineEdit(parent);
    inputs["airline"] = new QLineEdit(parent);
    inputs["fromCity"] = new QLineEdit(parent);
    inputs["toCity"] = new QLineEdit(parent);
    inputs["departureTime"] = new QDateTimeEdit(QDateTime::currentDateTime(), parent);
    inputs["arrivalTime"] = new QDateTimeEdit(QDateTime::currentDateTime().addSecs(3600), parent);
    inputs["totalSeats"] = new QSpinBox(parent);
    inputs["price"] = new QDoubleSpinBox(parent);
    
    static_cast<QSpinBox*>(inputs["totalSeats"])->setRange(1, 1000);
    static_cast<QDoubleSpinBox*>(inputs["price"])->setRange(0, 100000);
    static_cast<QDoubleSpinBox*>(inputs["price"])->setDecimals(2);
    
    return inputs;
}

void MainWindow::addInputsToLayout(QFormLayout* layout, const QMap<QString, QWidget*>& inputs)
{
    QMap<QString, QString> labels = {
        {"flightID", "航班号:"},
        {"airline", "航空公司:"},
        {"fromCity", "出发城市:"},
        {"toCity", "到达城市:"},
        {"departureTime", "起飞时间:"},
        {"arrivalTime", "降落时间:"},
        {"totalSeats", "总座位数:"},
        {"price", "票价:"}
    };
    
    for (auto it = inputs.begin(); it != inputs.end(); ++it) {
        layout->addRow(labels[it.key()], it.value());
    }
}

Flight MainWindow::createFlightFromInputs(const QMap<QString, QWidget*>& inputs)
{
    Flight flight;
    flight.flightID = static_cast<QLineEdit*>(inputs["flightID"])->text().toStdString();
    flight.airline = static_cast<QLineEdit*>(inputs["airline"])->text().toStdString();
    flight.fromCity = static_cast<QLineEdit*>(inputs["fromCity"])->text().toStdString();
    flight.toCity = static_cast<QLineEdit*>(inputs["toCity"])->text().toStdString();
    flight.departureTime = static_cast<QDateTimeEdit*>(inputs["departureTime"])->dateTime();
    flight.arrivalTime = static_cast<QDateTimeEdit*>(inputs["arrivalTime"])->dateTime();
    flight.totalSeats = static_cast<QSpinBox*>(inputs["totalSeats"])->value();
    flight.availableSeats = flight.totalSeats;
    flight.price = static_cast<QDoubleSpinBox*>(inputs["price"])->value();
    flight.status = FlightStatus::Normal;
    return flight;
}

void MainWindow::connectSignals()
{
    // 连接航班管理器的信号
    connect(flightManager, &FlightManager::flightAdded,
            this, &MainWindow::onFlightAdded);
    connect(flightManager, &FlightManager::flightRemoved,
            this, &MainWindow::onFlightRemoved);
    connect(flightManager, &FlightManager::flightStatusChanged,
            this, &MainWindow::onFlightStatusChanged);
    connect(flightManager, &FlightManager::dataFileChanged,
            this, &MainWindow::onDataFileChanged);
    // 移除 dataModified 信号连接，避免过多通知
    
    // 连接票务管理器的信号
    connect(ticketManager, &TicketManager::ticketPurchased,
            this, &MainWindow::onTicketPurchased);
    connect(ticketManager, &TicketManager::ticketRefunded,
            this, &MainWindow::onTicketRefunded);
    
    // 连接通知管理器的信号
    connect(notificationManager, &NotificationManager::notificationGenerated,
            this, &MainWindow::onNotificationReceived);
}

QTableWidget *MainWindow::createTable(const QStringList &headers)
{
    QTableWidget *table = new QTableWidget(this);
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);
    return table;
}

void MainWindow::updateFlightComboBox()
{
    ui->flightComboBox->clear();
    const QList<Flight*>& flights = flightManager->getAllFlights();
    for (Flight* flight : flights)
    {
        QString text = QString("%1 %2→%3 %4元 (余票:%5)")
            .arg(QString::fromStdString(flight->flightID))
            .arg(QString::fromStdString(flight->fromCity))
            .arg(QString::fromStdString(flight->toCity))
            .arg(flight->price)
            .arg(flight->availableSeats);
        ui->flightComboBox->addItem(text, QString::fromStdString(flight->flightID));
    }
}

void MainWindow::updateCityComboBoxes()
{
    QSet<QString> cities;
    const QList<Flight*>& flights = flightManager->getAllFlights();
    for (Flight* flight : flights)
    {
        cities.insert(QString::fromStdString(flight->fromCity));
        cities.insert(QString::fromStdString(flight->toCity));
    }
    
    QStringList cityList = cities.values();
    cityList.sort();
    
    ui->fromCityCombo->clear();
    ui->toCityCombo->clear();
    ui->fromCityCombo->addItems(cityList);
    ui->toCityCombo->addItems(cityList);
}

// 航班管理相关槽函数实现
void MainWindow::on_addFlightButton_clicked()
{
    showAddFlightDialog();
}

void MainWindow::on_deleteFlightButton_clicked()
{
    QList<QTableWidgetItem*> selectedItems = ui->flightTable->selectedItems();
    if (selectedItems.isEmpty())
    {
        QMessageBox::warning(this, "警告", "请先选择要删除的航班！");
        return;
    }
    
    int row = selectedItems.first()->row();
    QString flightID = ui->flightTable->item(row, 0)->text();
    
    if (QMessageBox::question(this, "确认删除",
        QString("确定要删除航班 %1 吗？").arg(flightID),
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        flightManager->deleteFlight(flightID);
    }
}

void MainWindow::on_importButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("选择航班数据文件"), "",
        tr("文本文件 (*.txt);;所有文件 (*)"));

    if (filename.isEmpty())
        return;

    // 询问用户是否要清空现有数据
    QMessageBox::StandardButton reply = QMessageBox::question(this, "导入数据","是否清空现有数据后导入？\n选择'否'将合并数据。",QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel) {
        return;
    }

    // 如果选择清空数据
    if (reply == QMessageBox::Yes) {
        flightManager->clearAllData();
        ticketManager->clearAllData();
    }

    // 加载新数据
    if (flightManager->loadFromFile(filename)) {
        QMessageBox::information(this, tr("成功"), tr("航班数据导入成功！"));
        // 更新界面
        updateFlightTable();
        updateFlightComboBox();
        updateCityComboBoxes();
        updateTicketTable();
    } else {
        QMessageBox::warning(this, tr("错误"), tr("航班数据导入失败！"));
    }
}

void MainWindow::on_refreshFlightButton_clicked()
{
    // 重新加载航班数据
    QString flightsPath = "D:\\myCollege\\CS class\\DataStructure\\oncomputer\\codes\\FightS\\flights.txt";
    if (flightManager->loadFromFileWithLock(flightsPath)) {
        updateFlightTable();
        updateFlightComboBox();
        updateCityComboBoxes();
        ui->statusbar->showMessage("航班数据已刷新", 2000);
    } else {
        QMessageBox::warning(this, "刷新失败", "无法重新加载航班数据！");
    }
}

// 票务操作相关槽函数实现
void MainWindow::on_purchaseButton_clicked()
{
    qDebug() << "=== 开始购票流程 ===";
    
    QString flightID = ui->flightComboBox->currentData().toString();
    QString passenger = ui->passengerNameEdit->text();
    
    qDebug() << "步骤1: 获取用户输入";
    qDebug() << "  - 选中的航班ID:" << flightID;
    qDebug() << "  - 乘客姓名:" << passenger;
    qDebug() << "  - 下拉框当前索引:" << ui->flightComboBox->currentIndex();
    qDebug() << "  - 下拉框当前文本:" << ui->flightComboBox->currentText();
    
    if (flightID.isEmpty() || passenger.isEmpty())
    {
        qDebug() << "错误: 输入验证失败 - 航班ID或乘客姓名为空";
        QMessageBox::warning(this, "警告", "请选择航班并输入乘客姓名！");
        return;
    }
    
    qDebug() << "步骤2: 验证航班是否存在";
    Flight* selectedFlight = flightManager->findFlight(flightID);
    if (!selectedFlight) {
        qDebug() << "错误: 找不到航班" << flightID;
        qDebug() << "  - 当前系统中的所有航班:";
        const QList<Flight*>& allFlights = flightManager->getAllFlights();
        for (int i = 0; i < allFlights.size(); ++i) {
            Flight* f = allFlights[i];
            qDebug() << "    [" << i << "]" << QString::fromStdString(f->flightID) 
                     << QString::fromStdString(f->fromCity) << "->" << QString::fromStdString(f->toCity);
        }
        QMessageBox::warning(this, "错误", "选中的航班不存在！");
        return;
    }
    
    qDebug() << "步骤3: 航班验证成功";
    qDebug() << "  - 航班号:" << QString::fromStdString(selectedFlight->flightID);
    qDebug() << "  - 航空公司:" << QString::fromStdString(selectedFlight->airline);
    qDebug() << "  - 出发城市:" << QString::fromStdString(selectedFlight->fromCity);
    qDebug() << "  - 到达城市:" << QString::fromStdString(selectedFlight->toCity);
    qDebug() << "  - 总座位数:" << selectedFlight->totalSeats;
    qDebug() << "  - 余票数量:" << selectedFlight->availableSeats;
    qDebug() << "  - 票价:" << selectedFlight->price;
    qDebug() << "  - 航班状态:" << static_cast<int>(selectedFlight->status);
    
    qDebug() << "步骤4: 调用购票管理器";
    Ticket* ticket = ticketManager->purchaseTicketWithLock(flightID, passenger);
    
    if (ticket)
    {
        qDebug() << "步骤5: 购票成功";
        qDebug() << "  - 生成的票号:" << QString::fromStdString(ticket->ticketID);
        qDebug() << "  - 票务乘客姓名:" << QString::fromStdString(ticket->passengerName);
        qDebug() << "  - 关联航班ID:" << QString::fromStdString(ticket->flight->flightID);
        qDebug() << "  - 关联航班出发城市:" << QString::fromStdString(ticket->flight->fromCity);
        qDebug() << "  - 关联航班到达城市:" << QString::fromStdString(ticket->flight->toCity);
        qDebug() << "  - 购票时间:" << ticket->purchaseTime.toString("yyyy-MM-dd hh:mm:ss");
        qDebug() << "  - 航班余票更新后:" << selectedFlight->availableSeats;
        
        QMessageBox::information(this, "购票成功", 
            QString("乘客 %1 成功购买航班 %2 的机票\n票号：%3\n出发城市：%4\n到达城市：%5")
            .arg(QString::fromStdString(ticket->passengerName))
            .arg(QString::fromStdString(ticket->flight->flightID))
            .arg(QString::fromStdString(ticket->ticketID))
            .arg(QString::fromStdString(ticket->flight->fromCity))
            .arg(QString::fromStdString(ticket->flight->toCity)));
        
        qDebug() << "步骤6: 更新界面显示";
            updateFlightTable();
            updateFlightComboBox();
        
        // 显示所有票务（包括刚购买的）
        qDebug() << "  - 购票成功，更新票务表格显示所有票务";
        updateTicketTable(); // 显示所有票务
        
        qDebug() << "步骤7: 保存票务数据到文件";
        QString ticketsPath = "D:\\myCollege\\CS class\\DataStructure\\oncomputer\\codes\\FightS\\tickets.txt";
        bool saveResult = ticketManager->saveToFile(ticketsPath);
        qDebug() << "  - 保存结果:" << (saveResult ? "成功" : "失败");
        
        // 验证票务管理器中的数据
        QList<Ticket*> allTickets = ticketManager->getAllTickets();
        qDebug() << "  - 票务管理器中总票数:" << allTickets.size();
        for (int i = 0; i < allTickets.size(); ++i) {
            Ticket* t = allTickets[i];
            qDebug() << "    票" << i << ":" << QString::fromStdString(t->ticketID) 
                     << QString::fromStdString(t->flight->flightID) 
                     << QString::fromStdString(t->passengerName);
        }
        
        // 清空输入框
        ui->passengerNameEdit->clear();
        qDebug() << "步骤8: 购票流程完成";
    }
    else
    {
        qDebug() << "错误: 购票失败";
        qDebug() << "  - 可能原因: 余票不足或其他验证失败";
        qDebug() << "  - 当前航班余票:" << selectedFlight->availableSeats;
        QMessageBox::warning(this, "购票失败", "购票失败，请检查航班是否有余票！");
    }
    
    qDebug() << "=== 购票流程结束 ===\n";
}

void MainWindow::on_refundButton_clicked()
{
    QString ticketID = ui->ticketIDEdit->text();
    if (ticketID.isEmpty())
    {
        QMessageBox::warning(this, "警告", "请输入票号！");
        return;
    }
    
    if (ticketManager->refundTicket(ticketID))
    {
        QMessageBox::information(this, "退票成功", "退票成功！");
        updateTicketTable();
        updateFlightTable(); // 更新航班表格以显示最新的余票信息
        updateFlightComboBox(); // 更新下拉框显示余票信息
        
        // 自动保存数据到文件
            QString flightsPath = "D:\\myCollege\\CS class\\DataStructure\\oncomputer\\codes\\FightS\\flights.txt";
            QString ticketsPath = "D:\\myCollege\\CS class\\DataStructure\\oncomputer\\codes\\FightS\\tickets.txt";
            flightManager->saveToFileWithLock(flightsPath);
            ticketManager->saveToFile(ticketsPath);
        
        // 清空输入框
        ui->ticketIDEdit->clear();
    }
    else
    {
        QMessageBox::warning(this, "退票失败", "退票失败，请检查票号是否正确！");
    }
}

void MainWindow::on_reserveButton_clicked()
{
    QString passengerName = ui->passengerNameEdit->text();
    if (passengerName.isEmpty())
    {
        QMessageBox::warning(this, "警告", "请输入乘客姓名！");
        return;
    }
    
    int index = ui->flightComboBox->currentIndex();
    if (index < 0)
    {
        QMessageBox::warning(this, "警告", "请选择航班！");
        return;
    }
    
    QString flightID = ui->flightComboBox->currentData().toString();
    Flight* flight = flightManager->findFlight(flightID);
    if (flight && flight->availableSeats > 0) {
        QMessageBox::information(this, "提示", "该航班还有余票，请直接购票，无需预约！");
        return;
    }
    ticketManager->addReservation(flightID, passengerName);
    QMessageBox::information(this, "预约成功",
        QString("已成功预约航班 %1 的机票").arg(flightID));
}

// 查询相关槽函数实现
void MainWindow::on_searchButton_clicked()
{
    QString searchText = ui->searchEdit->text();
    if (searchText.isEmpty())
    {
        updateTicketTable();
        return;
    }

    QList<Ticket*> tickets;
    switch (ui->searchTypeCombo->currentIndex()) {
        case 0: // 按乘客查询
            tickets = ticketManager->findTicketsByPassenger(searchText);
            break;
        case 1: // 按航班查询
            tickets = ticketManager->findTicketsByFlight(searchText);
            break;
        case 2: // 按票号查询
            if (Ticket* ticket = ticketManager->findTicketByID(searchText)) {
                tickets.append(ticket);
            }
            break;
    }

    updateTicketTable(tickets);
}

void MainWindow::on_processReserveButton_clicked()
{
    ticketManager->processReservations();
    updateTicketTable();
    updateFlightTable(); // 更新航班表格以显示最新的余票信息
    updateFlightComboBox(); // 更新下拉框显示余票信息
}

void MainWindow::on_refreshTicketButton_clicked()
{
    // 刷新票务数据和航班数据
    QString flightsPath = "D:\\myCollege\\CS class\\DataStructure\\oncomputer\\codes\\FightS\\flights.txt";
    if (flightManager->loadFromFileWithLock(flightsPath)) {
        updateTicketTable();
        updateFlightTable();
        updateFlightComboBox();
        updateCityComboBoxes();
        ui->statusbar->showMessage("票务数据已刷新", 2000);
    } else {
        QMessageBox::warning(this, "刷新失败", "无法重新加载数据！");
    }
}

// 安全的字符串转换函数
QString MainWindow::safeFromStdString(const std::string& str, const QString& fallback)
{
    try {
        if (str.empty()) {
            return fallback;
        }
        return QString::fromStdString(str);
    } catch (...) {
        qDebug() << "字符串转换失败，使用默认值：" << fallback;
        return fallback;
    }
}

void MainWindow::updateTicketTable(const QList<Ticket*>& tickets)
{
    qDebug() << "  [UI] 开始更新票务表格";
    
    // 简化函数，减少调试输出，避免编码问题
    try {
        // 检查UI组件是否有效
        if (!ui || !ui->ticketTable) {
            qDebug() << "    - 错误: UI组件无效";
            return;
        }
        
        // 如果没有传入票务列表，则获取所有票务
        QList<Ticket*> displayTickets = tickets.isEmpty() ? 
            ticketManager->getAllTickets() : tickets;

        qDebug() << "    - 传入的票务数量:" << tickets.size();
        qDebug() << "    - 实际显示的票务数量:" << displayTickets.size();
        
        if (tickets.isEmpty()) {
            qDebug() << "    - 使用所有票务数据（从TicketManager获取）";
        } else {
            qDebug() << "    - 使用指定的票务数据";
        }

        // 清空表格
        ui->ticketTable->setRowCount(0);
        qDebug() << "    - 表格已清空";

        // 添加票务信息到表格
        int validTicketCount = 0;
        for (int i = 0; i < displayTickets.size(); ++i) {
            const Ticket* ticket = displayTickets[i];
            qDebug() << "    - 处理票务" << i << ":";
            
            if (!ticket || !ticket->flight) {
                qDebug() << "      * 跳过无效记录（ticket或flight为空）";
                continue; // 跳过无效记录
            }
            
            try {
                // 使用安全函数转换字符串
                QString ticketID = safeFromStdString(ticket->ticketID, "N/A");
                QString flightID = safeFromStdString(ticket->flight->flightID, "N/A");
                QString passengerName = safeFromStdString(ticket->passengerName, "N/A");
                QString fromCity = safeFromStdString(ticket->flight->fromCity, "N/A");
                QString toCity = safeFromStdString(ticket->flight->toCity, "N/A");
                QString purchaseTime = ticket->purchaseTime.toString("yyyy-MM-dd hh:mm:ss");
                
                qDebug() << "      * 票号:" << ticketID;
                qDebug() << "      * 航班:" << flightID;
                qDebug() << "      * 乘客:" << passengerName;
                qDebug() << "      * 路线:" << fromCity << "->" << toCity;
                qDebug() << "      * 购票时间:" << purchaseTime;
                
                int row = ui->ticketTable->rowCount();
                ui->ticketTable->insertRow(row);

                // 设置票务信息
                ui->ticketTable->setItem(row, 0, new QTableWidgetItem(ticketID));
                ui->ticketTable->setItem(row, 1, new QTableWidgetItem(flightID));
                ui->ticketTable->setItem(row, 2, new QTableWidgetItem(passengerName));
                ui->ticketTable->setItem(row, 3, new QTableWidgetItem(fromCity));
                ui->ticketTable->setItem(row, 4, new QTableWidgetItem(toCity));
                ui->ticketTable->setItem(row, 5, new QTableWidgetItem(purchaseTime));
                
                validTicketCount++;
                qDebug() << "      * 成功添加到表格第" << row << "行";
                
            } catch (...) {
                qDebug() << "      * 异常: 跳过有问题的记录";
                continue;
            }
        }
        
        qDebug() << "    - 表格更新完成，有效票务数量:" << validTicketCount;
        qDebug() << "    - 表格总行数:" << ui->ticketTable->rowCount();
        
    } catch (...) {
        qDebug() << "    - 异常: 更新票务表格时发生错误";
        QMessageBox::critical(this, "Error", "Failed to update ticket table!");
    }
}

// 信号处理槽函数实现
void MainWindow::onFlightAdded(Flight* flight)
{
    updateFlightTable();
    updateFlightComboBox();
    updateCityComboBoxes();
}

void MainWindow::onFlightRemoved(QString flightID)
{
    updateFlightTable();
    updateFlightComboBox();
    updateCityComboBoxes();
}

void MainWindow::onFlightStatusChanged(QString flightID, FlightStatus status)
{
    updateFlightTable();
}

void MainWindow::onTicketPurchased(Ticket* ticket)
{
    // 暂时禁用这个信号处理，避免重复提示
    // QMessageBox::information(this, "购票成功",
    //     QString("乘客 %1 成功购买航班 %2 的机票\n票号：%3")
    //     .arg(QString::fromStdString(ticket->passengerName))
    //     .arg(QString::fromStdString(ticket->flight->flightID))
    //     .arg(QString::fromStdString(ticket->ticketID)));
    updateFlightTable();
    updateFlightComboBox();
    updateTicketTable(); // 添加票务表格更新
}

void MainWindow::onTicketRefunded(QString ticketID)
{
    QMessageBox::information(this, "退票成功",
        QString("票号 %1 已成功退票").arg(ticketID));
    updateFlightTable();
}

void MainWindow::onNotificationReceived(QString message)
{
    ui->statusbar->showMessage(message, 5000); // 显示5秒
}

void MainWindow::onDataFileChanged()
{
    // 重新加载数据并更新UI
    updateFlightTable();
    updateTicketTable();
    updateFlightComboBox();
    updateCityComboBoxes();
    
    // 显示同步提示
    showMessage("数据同步", "数据已更新！", QMessageBox::Information);
}

void MainWindow::onDataModified()
{
    // 数据发生修改，只显示状态栏提示，不显示QMessageBox
    // 避免频繁弹出对话框影响用户体验
    ui->statusbar->showMessage("数据已修改并自动保存", 3000);
}

void MainWindow::on_searchButton1_clicked()
{
    // 获取出发城市和到达城市
    QString fromCity = ui->fromCityCombo->currentText();
    QString toCity = ui->toCityCombo->currentText();
    
    // 获取时间范围
    QDateTime startTime = ui->startDateEdit->dateTime();
    QDateTime endTime = ui->endDateEdit->dateTime();
    
    // 验证输入
    if (fromCity.isEmpty() || toCity.isEmpty()) {
        QMessageBox::warning(this, "警告", "请选择出发城市和到达城市！");
        return;
    }
    
    if (startTime > endTime) {
        QMessageBox::warning(this, "警告", "开始时间不能晚于结束时间！");
        return;
    }
    
    // 获取所有航班
    const QList<Flight*>& allFlights = flightManager->getAllFlights();
    QList<Flight*> matchedFlights;
    
    // 筛选符合条件的航班
    for (Flight* flight : allFlights) {
        // 检查城市匹配
        if (QString::fromStdString(flight->fromCity) != fromCity || QString::fromStdString(flight->toCity) != toCity) {
            continue;
        }
        
        // 检查时间范围
        if (flight->departureTime < startTime || flight->departureTime > endTime) {
            continue;
        }
        
        // 检查航班状态
        if (flight->status != FlightStatus::Normal) {
            continue;
        }
        
        matchedFlights.append(flight);
    }
    
    // 更新结果表格
    if (matchedFlights.isEmpty()) {
        QMessageBox::information(this, "查询结果", "未找到符合条件的航班！");
        ui->resultTable->setVisible(false);
    } else {
        updateResultTable(matchedFlights);
        QMessageBox::information(this, "查询结果", 
            QString("找到 %1 个符合条件的航班").arg(matchedFlights.size()));
    }
}

void MainWindow::on_sortByPriceButton_clicked()
{
    // 获取当前显示的结果
    QList<Flight*> flights;
    for (int i = 0; i < ui->resultTable->rowCount(); ++i) {
        QString flightID = ui->resultTable->item(i, 0)->text();
        if (Flight* flight = flightManager->findFlight(flightID)) {
            flights.append(flight);
        }
    }
    
    // 按价格排序
    std::sort(flights.begin(), flights.end(), 
        [](const Flight* a, const Flight* b) {
            return a->price < b->price;
        });
    
    // 更新显示
    updateResultTable(flights);
}

void MainWindow::on_sortByTimeButton_clicked()
{
    // 获取当前显示的结果
    QList<Flight*> flights;
    for (int i = 0; i < ui->resultTable->rowCount(); ++i) {
        QString flightID = ui->resultTable->item(i, 0)->text();
        if (Flight* flight = flightManager->findFlight(flightID)) {
            flights.append(flight);
        }
    }
    
    // 按起飞时间排序
    std::sort(flights.begin(), flights.end(), 
        [](const Flight* a, const Flight* b) {
            return a->departureTime < b->departureTime;
        });
    
    // 更新显示
    updateResultTable(flights);
}

void MainWindow::on_refreshSearchButton_clicked()
{
    // 刷新查询数据
    QString flightsPath = "D:\\myCollege\\CS class\\DataStructure\\oncomputer\\codes\\FightS\\flights.txt";
    if (flightManager->loadFromFileWithLock(flightsPath)) {
        updateCityComboBoxes();
        
        // 如果有查询结果，重新执行查询
        if (ui->resultTable->isVisible() && ui->resultTable->rowCount() > 0) {
            on_searchButton1_clicked();
        }
        
        ui->statusbar->showMessage("查询数据已刷新", 2000);
    } else {
        QMessageBox::warning(this, "刷新失败", "无法重新加载数据！");
    }
}

// 添加保存按钮的槽函数
void MainWindow::on_saveButton_clicked()
{
    try {
        bool success = true;
        QString errorMessage;
        
        // 先检查数据状态
        int flightCount = flightManager->getAllFlights().size();
        qDebug() << "准备保存，当前航班数量：" << flightCount;
        
        if (flightCount == 0) {
            QMessageBox::warning(this, "警告", "没有航班数据可保存！请先加载或添加航班数据。");
        return;
    }

        // 使用FlightManager的保存方法
        QString flightsPath = "D:\\myCollege\\CS class\\DataStructure\\oncomputer\\codes\\FightS\\flights.txt";
        qDebug() << "开始保存航班数据到：" << flightsPath;
        
        if (!flightManager->saveToFileWithLock(flightsPath)) {
            success = false;
            errorMessage += "航班数据保存失败！\n";
            qDebug() << "航班数据保存失败";
        } else {
            qDebug() << "航班数据保存成功";
        }
        
        // 使用TicketManager的保存方法
    QString ticketsPath = "D:\\myCollege\\CS class\\DataStructure\\oncomputer\\codes\\FightS\\tickets.txt";
        qDebug() << "开始保存票务数据到：" << ticketsPath;
        
        if (!ticketManager->saveToFile(ticketsPath)) {
            success = false;
            errorMessage += "票务数据保存失败！\n";
            qDebug() << "票务数据保存失败";
        } else {
            qDebug() << "票务数据保存成功";
        }
        
        // 获取票务数量
        int ticketCount = ticketManager->getAllTickets().size();
        qDebug() << "票务数量：" << ticketCount;
        
        if (success) {
            QMessageBox::information(this, "成功", 
                QString("数据保存成功！\n航班数量：%1\n票务数量：%2")
                .arg(flightCount)
                .arg(ticketCount));
        } else {
            QMessageBox::warning(this, "警告", errorMessage);
        }
        
    } catch (const std::exception& e) {
        qDebug() << "保存时发生标准异常：" << e.what();
        QMessageBox::critical(this, "错误", QString("保存数据时发生错误：%1").arg(e.what()));
    } catch (...) {
        qDebug() << "保存时发生未知异常";
        QMessageBox::critical(this, "错误", "保存数据时发生未知错误！");
    }
}

// 航班状态管理功能实现
void MainWindow::on_changeFlightStatusButton_clicked()
{
    showFlightStatusDialog();
}

void MainWindow::showFlightStatusDialog()
{
    // 获取当前选中的航班
    int currentRow = ui->flightTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "警告", "请先选择要修改状态的航班！");
        return;
    }

    QString flightID = ui->flightTable->item(currentRow, 0)->text();
    Flight* flight = flightManager->findFlight(flightID);
    if (!flight) {
        QMessageBox::warning(this, "错误", "找不到指定的航班！");
        return;
    }
    
    // 创建状态选择对话框
    QDialog dialog(this);
    dialog.setWindowTitle("修改航班状态 - " + flightID);
    dialog.setFixedSize(400, 300);
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    // 当前状态显示
    QString currentStatusText;
    switch (flight->status) {
        case FlightStatus::Normal: currentStatusText = "正常"; break;
        case FlightStatus::Delayed: currentStatusText = "延误"; break;
        case FlightStatus::Cancelled: currentStatusText = "取消"; break;
    }
    
    QLabel* currentLabel = new QLabel(QString("当前状态：%1").arg(currentStatusText));
    layout->addWidget(currentLabel);
    
    // 新状态选择
    QLabel* newLabel = new QLabel("新状态：");
    layout->addWidget(newLabel);
    
    QComboBox* statusCombo = new QComboBox();
    statusCombo->addItem("正常", static_cast<int>(FlightStatus::Normal));
    statusCombo->addItem("延误", static_cast<int>(FlightStatus::Delayed));
    statusCombo->addItem("取消", static_cast<int>(FlightStatus::Cancelled));
    
    // 设置当前状态为默认选中
    statusCombo->setCurrentIndex(static_cast<int>(flight->status));
    layout->addWidget(statusCombo);
    
    // 延误时间输入（仅延误状态可用）
    QLabel* delayLabel = new QLabel("延误时间（小时）：");
    QSpinBox* delayHours = new QSpinBox();
    delayHours->setRange(0, 24);
    delayHours->setValue(0);
    delayHours->setEnabled(false);
    
    layout->addWidget(delayLabel);
    layout->addWidget(delayHours);
    
    // 状态改变时启用/禁用延误时间输入
    connect(statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [delayHours](int index) {
        delayHours->setEnabled(index == static_cast<int>(FlightStatus::Delayed));
    });
    
    // 按钮
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        FlightStatus newStatus = static_cast<FlightStatus>(statusCombo->currentData().toInt());
        
        // 如果状态没有改变，直接返回
        if (newStatus == flight->status) {
            return;
        }
        
        // 如果是延误，更新航班时间
        if (newStatus == FlightStatus::Delayed && delayHours->value() > 0) {
            flight->departureTime = flight->departureTime.addSecs(delayHours->value() * 3600);
            flight->arrivalTime = flight->arrivalTime.addSecs(delayHours->value() * 3600);
        }
        
        // 更新航班状态
        flightManager->setFlightStatus(flightID, newStatus);
        
        // 通知乘客
        notifyPassengersOfStatusChange(flightID, newStatus);
        
        // 更新界面
        updateFlightTable();
        
        // 显示成功消息（不需要手动保存，因为setFlightStatus已经自动保存了）
        QMessageBox::information(this, "成功", 
            QString("航班 %1 状态已更新为：%2\n数据已自动保存，其他客户端将同步更新").arg(flightID).arg(statusCombo->currentText()));
    }
}

void MainWindow::notifyPassengersOfStatusChange(const QString& flightID, FlightStatus newStatus)
{
    Flight* originalFlight = flightManager->findFlight(flightID);
    if (!originalFlight) return;
    
    // 获取该航班的所有乘客数量
    QList<Ticket*> passengers = ticketManager->getTicketsByFlightID(flightID);
    int passengerCount = passengers.size();
    
    QString statusText;
    QString messageTitle;
    
    switch (newStatus) {
        case FlightStatus::Delayed:
            statusText = "延误";
            messageTitle = "航班延误通知";
            break;
        case FlightStatus::Cancelled:
            statusText = "取消";
            messageTitle = "航班取消通知";
            break;
        case FlightStatus::Normal:
            statusText = "恢复正常";
            messageTitle = "航班恢复正常通知";
            break;
    }
    
    // 构建基本信息
    QString notificationMessage = QString("航班 %1 (%2 → %3) 状态已更改为【%4】\n\n")
        .arg(flightID)
        .arg(QString::fromStdString(originalFlight->fromCity))
        .arg(QString::fromStdString(originalFlight->toCity))
        .arg(statusText);
        
    notificationMessage += QString("起飞时间：%1\n")
        .arg(originalFlight->departureTime.toString("yyyy-MM-dd hh:mm"));
    notificationMessage += QString("到达时间：%1\n")
        .arg(originalFlight->arrivalTime.toString("yyyy-MM-dd hh:mm"));
    notificationMessage += QString("受影响乘客数：%1人\n")
        .arg(passengerCount);
    
    // 如果航班取消或延误，查找替代方案
    if (newStatus == FlightStatus::Cancelled || newStatus == FlightStatus::Delayed) {
        try {
            qDebug() << "航班" << statusText << "，开始查找替代航班";
            QList<Flight*> alternatives = findAlternativeFlights(flightID);
            notificationMessage += "\n推荐的替代航班：\n";
            if (!alternatives.isEmpty()) {
                int count = 0;
                for (Flight* altFlight : alternatives) {
                    if (!altFlight) continue;
                    if (count >= 3) break;
                    notificationMessage += QString("%1. 航班 %2: %3 → %4\n")
                        .arg(count + 1)
                        .arg(QString::fromStdString(altFlight->flightID))
                        .arg(QString::fromStdString(altFlight->fromCity))
                        .arg(QString::fromStdString(altFlight->toCity));
                    notificationMessage += QString("   起飞：%1，价格：￥%2\n")
                        .arg(altFlight->departureTime.toString("yyyy-MM-dd hh:mm"))
                        .arg(altFlight->price, 0, 'f', 2);
                    count++;
                }
            } else {
                notificationMessage += "抱歉，暂时没有找到合适的替代航班。\n";
            }
        } catch (const std::exception& e) {
            qDebug() << "显示替代航班时发生异常：" << e.what();
            notificationMessage += "查找替代航班时发生错误。\n";
        } catch (...) {
            qDebug() << "显示替代航班时发生未知异常";
            notificationMessage += "查找替代航班时发生错误。\n";
        }
    }
    
    // 直接显示简化的消息框
    QMessageBox::information(this, messageTitle, notificationMessage);
    
    // 发送系统通知
    onNotificationReceived(QString("航班 %1 %2，影响 %3 位乘客")
                          .arg(flightID).arg(statusText).arg(passengerCount));
}

QList<Flight*> MainWindow::findAlternativeFlights(const QString& originalFlightID)
{
    try {
        Flight* originalFlight = flightManager->findFlight(originalFlightID);
        if (!originalFlight) {
            qDebug() << "找不到原航班：" << originalFlightID;
            return QList<Flight*>();
        }
        
        QString fromCity = QString::fromStdString(originalFlight->fromCity);
        QString toCity = QString::fromStdString(originalFlight->toCity);
        
        qDebug() << "查找替代航班：从" << fromCity << "到" << toCity;
        
        // 查找相同路线的其他航班
        QList<Flight*> sameRouteFlights = flightManager->searchByCity(fromCity, toCity);
        QList<Flight*> alternatives;
        
        qDebug() << "找到相同路线航班数量：" << sameRouteFlights.size();
        
        QDateTime originalDeparture = originalFlight->departureTime;
        
        // 优先选择时间相近的航班（前后24小时内）
        for (Flight* flight : sameRouteFlights) {
            if (!flight) continue; // 安全检查
            
            // 排除原航班和非正常状态的航班
            QString currentFlightID = QString::fromStdString(flight->flightID);
            if (currentFlightID == originalFlightID || 
                flight->status != FlightStatus::Normal ||
                flight->availableSeats <= 0) {
                continue;
            }
            
            // 优先选择时间相近的航班（前后24小时内）
            qint64 timeDiff = qAbs(originalDeparture.secsTo(flight->departureTime));
            if (timeDiff <= 24 * 3600) { // 24小时内
                alternatives.append(flight);
            }
        }
        
        // 如果没有找到24小时内的航班，放宽到一周内
        if (alternatives.isEmpty()) {
            for (Flight* flight : sameRouteFlights) {
                if (!flight) continue; // 安全检查
                
                QString currentFlightID = QString::fromStdString(flight->flightID);
                if (currentFlightID == originalFlightID || 
                    flight->status != FlightStatus::Normal ||
                    flight->availableSeats <= 0) {
                    continue;
                }
                
                qint64 timeDiff = qAbs(originalDeparture.secsTo(flight->departureTime));
                if (timeDiff <= 7 * 24 * 3600) { // 一周内
                    alternatives.append(flight);
                }
            }
        }
        
        qDebug() << "找到替代航班数量：" << alternatives.size();
        
        // 按时间差排序，优先推荐时间最接近的
        if (!alternatives.isEmpty()) {
            std::sort(alternatives.begin(), alternatives.end(), 
                [originalDeparture](Flight* a, Flight* b) {
                    qint64 diffA = qAbs(originalDeparture.secsTo(a->departureTime));
                    qint64 diffB = qAbs(originalDeparture.secsTo(b->departureTime));
                    return diffA < diffB;
                });
        }
        
        return alternatives;
    } catch (const std::exception& e) {
        qDebug() << "查找替代航班时发生异常：" << e.what();
        return QList<Flight*>();
    } catch (...) {
        qDebug() << "查找替代航班时发生未知异常";
        return QList<Flight*>();
    }
}

// 添加关闭窗口事件处理
void MainWindow::refreshAllData()
{
    QString flightsPath = "D:\\myCollege\\CS class\\DataStructure\\oncomputer\\codes\\FightS\\flights.txt";
    
    try {
        if (flightManager->loadFromFileWithLock(flightsPath)) {
            updateFlightTable();
            updateFlightComboBox();
            updateCityComboBoxes();
            updateTicketTable();
            ui->statusbar->showMessage("所有数据已刷新", 2000);
        } else {
            QMessageBox::warning(this, "刷新失败", "无法重新加载数据！");
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "错误", QString("刷新数据时发生错误：%1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(this, "错误", "刷新数据时发生未知错误！");
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // 询问用户是否保存数据
    QMessageBox::StandardButton reply = QMessageBox::question(this, "保存数据",
        "是否保存当前数据？",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Yes) {
        // 保存数据
        on_saveButton_clicked();
        event->accept();
    } else if (reply == QMessageBox::No) {
        // 不保存直接关闭
        event->accept();
    } else {
        // 取消关闭
        event->ignore();
    }
}

void MainWindow::showSyncMessageBox(const QString& type)
{
    QString msg;
    if (type == "flight") {
        msg = "检测到其他客户端修改了航班信息（如状态、增删航班等），已自动同步。";
    } else if (type == "ticket") {
        msg = "检测到其他客户端修改了票务信息（如购票、退票等），已自动同步。";
    } else {
        msg = "检测到其他客户端有数据变更，已自动同步。";
    }
    QMessageBox::information(this, "同步通知", msg);
}

void MainWindow::initializeUI()
{
    // 设置窗口标题和大小
    setWindowTitle("航班管理系统");
    resize(1200, 800);
    
    // 设置表格属性
    setupTables();
    
    // 设置样式
    setupStyles();
    
    // 初始化查询类型下拉框
    ui->searchTypeCombo->clear();
    ui->searchTypeCombo->addItem("按乘客查询");
    ui->searchTypeCombo->addItem("按航班查询");
    ui->searchTypeCombo->addItem("按票号查询");
    
    // 更新下拉框
    updateFlightComboBox();
    updateCityComboBoxes();
    
    // 连接菜单动作
    connect(ui->actionImport, &QAction::triggered, this, &MainWindow::on_importButton_clicked);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::on_saveButton_clicked);
    
    // 启动通知监控
    notificationManager->startMonitoring();
}



