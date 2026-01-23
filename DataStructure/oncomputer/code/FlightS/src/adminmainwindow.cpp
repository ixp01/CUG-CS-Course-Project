#include "../include/adminmainwindow.h"
#include "../include/addflightdialog.h"
#include "ui_admin_main.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDate>
#include <QScreen>
#include <QApplication>

AdminMainWindow::AdminMainWindow(int managerId, const QString& username, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AdminMainWindow)
    , currentManagerId(managerId)
    , currentUsername(username)
{
    ui->setupUi(this);
    setupUI();
    setupTableHeaders();
    
    // 连接信号槽
    connect(ui->addFlightBtn, &QPushButton::clicked, this, &AdminMainWindow::onAddFlight);
    connect(ui->editFlightBtn, &QPushButton::clicked, this, &AdminMainWindow::onEditFlight);
    connect(ui->deleteFlightBtn, &QPushButton::clicked, this, &AdminMainWindow::onDeleteFlight);
    connect(ui->refreshFlightBtn, &QPushButton::clicked, this, &AdminMainWindow::onRefreshFlights);
    connect(ui->searchFlightBtn, &QPushButton::clicked, this, &AdminMainWindow::onSearchFlight);
    connect(ui->flightTable, &QTableWidget::cellDoubleClicked, this, &AdminMainWindow::onFlightDoubleClicked);
    
    connect(ui->viewUserBtn, &QPushButton::clicked, this, &AdminMainWindow::onViewUser);
    connect(ui->resetPasswordBtn, &QPushButton::clicked, this, &AdminMainWindow::onResetPassword);
    connect(ui->refreshUserBtn, &QPushButton::clicked, this, &AdminMainWindow::onRefreshUsers);
    connect(ui->searchUserBtn, &QPushButton::clicked, this, &AdminMainWindow::onSearchUser);
    connect(ui->userTable, &QTableWidget::cellDoubleClicked, this, &AdminMainWindow::onUserDoubleClicked);
    
    connect(ui->generateStatsBtn, &QPushButton::clicked, this, &AdminMainWindow::onGenerateStats);
    connect(ui->exportStatsBtn, &QPushButton::clicked, this, &AdminMainWindow::onExportStats);
    
    connect(ui->backupDbBtn, &QPushButton::clicked, this, &AdminMainWindow::onBackupDatabase);
    connect(ui->restoreDbBtn, &QPushButton::clicked, this, &AdminMainWindow::onRestoreDatabase);
    connect(ui->optimizeDbBtn, &QPushButton::clicked, this, &AdminMainWindow::onOptimizeDatabase);
    connect(ui->refreshLogBtn, &QPushButton::clicked, this, &AdminMainWindow::onRefreshLog);
    connect(ui->clearLogBtn, &QPushButton::clicked, this, &AdminMainWindow::onClearLog);
    connect(ui->exportLogBtn, &QPushButton::clicked, this, &AdminMainWindow::onExportLog);
    
    // 菜单操作
    connect(ui->actionImportData, &QAction::triggered, this, &AdminMainWindow::onImportData);
    connect(ui->actionExportData, &QAction::triggered, this, &AdminMainWindow::onExportData);
    connect(ui->actionPreferences, &QAction::triggered, this, &AdminMainWindow::onPreferences);
    connect(ui->actionChangePassword, &QAction::triggered, this, &AdminMainWindow::onChangePassword);
    connect(ui->actionAbout, &QAction::triggered, this, &AdminMainWindow::onAbout);
    connect(ui->actionUserManual, &QAction::triggered, this, &AdminMainWindow::onUserManual);
    connect(ui->actionExit, &QAction::triggered, this, &AdminMainWindow::onExit);
    
    // 标签页切换时刷新数据
    connect(ui->tabWidget, &QTabWidget::currentChanged, 
            [this](int index) {
        switch (index) {
            case 0: // 航班管理
                loadFlights();
                break;
            case 1: // 用户管理
                loadUsers();
                break;
            case 2: // 统计报表
                // 初始化统计日期
                ui->statsFromDate->setDate(QDate::currentDate().addMonths(-1));
                ui->statsToDate->setDate(QDate::currentDate());
                break;
            case 3: // 系统管理
                updateSystemLog();
                break;
        }
    });
    
    // 初始加载数据
    loadFlights();
}

AdminMainWindow::~AdminMainWindow()
{
    delete ui;
}

void AdminMainWindow::setupUI()
{
    setWindowTitle(QString("航班管理系统 - 管理员: %1").arg(currentUsername));
    
    // 窗口居中
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    
    // 设置日期编辑器
    ui->statsFromDate->setDate(QDate::currentDate().addMonths(-1));
    ui->statsToDate->setDate(QDate::currentDate());
}

void AdminMainWindow::setupTableHeaders()
{
    // 航班表头
    QStringList flightHeaders = {"航班号", "航空公司", "出发地", "目的地", 
                                "起飞时间", "到达时间", "总座位", "余票", "价格", "状态"};
    ui->flightTable->setColumnCount(flightHeaders.size());
    ui->flightTable->setHorizontalHeaderLabels(flightHeaders);
    ui->flightTable->horizontalHeader()->setStretchLastSection(true);
    ui->flightTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->flightTable->setAlternatingRowColors(true);
    ui->flightTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 用户表头
    QStringList userHeaders = {"用户ID", "用户名", "邮箱", "电话", "用户类型", "注册时间"};
    ui->userTable->setColumnCount(userHeaders.size());
    ui->userTable->setHorizontalHeaderLabels(userHeaders);
    ui->userTable->horizontalHeader()->setStretchLastSection(true);
    ui->userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->userTable->setAlternatingRowColors(true);
    ui->userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void AdminMainWindow::loadFlights()
{
    currentFlights = DatabaseManager::instance().getAllFlights();
    displayFlights(currentFlights);
    ui->statusbar->showMessage(QString("已加载 %1 个航班").arg(currentFlights.size()), 3000);
}

void AdminMainWindow::loadUsers()
{
    currentUsers = DatabaseManager::instance().getAllUsers();
    displayUsers(currentUsers);
    ui->statusbar->showMessage(QString("已加载 %1 个用户").arg(currentUsers.size()), 3000);
}

void AdminMainWindow::displayFlights(const QVector<Flight>& flights)
{
    ui->flightTable->setRowCount(flights.size());
    
    for (int i = 0; i < flights.size(); ++i) {
        const Flight& flight = flights[i];
        
        ui->flightTable->setItem(i, COL_FLIGHT_NUMBER, new QTableWidgetItem(flight.flightNumber));
        ui->flightTable->setItem(i, COL_AIRLINE, new QTableWidgetItem(flight.airline));
        ui->flightTable->setItem(i, COL_DEPARTURE_CITY, new QTableWidgetItem(flight.departureCity));
        ui->flightTable->setItem(i, COL_ARRIVAL_CITY, new QTableWidgetItem(flight.arrivalCity));
        ui->flightTable->setItem(i, COL_DEPARTURE_TIME, 
                                new QTableWidgetItem(flight.departureTime.toString("yyyy-MM-dd hh:mm")));
        ui->flightTable->setItem(i, COL_ARRIVAL_TIME, 
                                new QTableWidgetItem(flight.arrivalTime.toString("yyyy-MM-dd hh:mm")));
        ui->flightTable->setItem(i, COL_TOTAL_SEATS, 
                                new QTableWidgetItem(QString::number(flight.totalSeats)));
        ui->flightTable->setItem(i, COL_AVAILABLE_SEATS, 
                                new QTableWidgetItem(QString::number(flight.availableSeats)));
        ui->flightTable->setItem(i, COL_PRICE, 
                                new QTableWidgetItem(QString("¥%1").arg(flight.price, 0, 'f', 0)));
        ui->flightTable->setItem(i, COL_STATUS, new QTableWidgetItem(flight.status));
        
        // 设置状态颜色
        QTableWidgetItem* statusItem = ui->flightTable->item(i, COL_STATUS);
        if (flight.status == Constants::FLIGHT_DELAYED) {
            statusItem->setBackground(QColor(255, 193, 7)); // 黄色
        } else if (flight.status == Constants::FLIGHT_CANCELLED) {
            statusItem->setBackground(QColor(220, 53, 69)); // 红色
            statusItem->setForeground(QColor(255, 255, 255));
        }
    }
    
    // 调整列宽
    ui->flightTable->resizeColumnsToContents();
}

void AdminMainWindow::displayUsers(const QVector<User>& users)
{
    ui->userTable->setRowCount(users.size());
    
    for (int i = 0; i < users.size(); ++i) {
        const User& user = users[i];
        
        ui->userTable->setItem(i, COL_USER_ID, new QTableWidgetItem(QString::number(user.userId)));
        ui->userTable->setItem(i, COL_USERNAME, new QTableWidgetItem(user.username));
        ui->userTable->setItem(i, COL_EMAIL, new QTableWidgetItem(user.email));
        ui->userTable->setItem(i, COL_PHONE, new QTableWidgetItem(user.phone));
        ui->userTable->setItem(i, COL_USER_TYPE, new QTableWidgetItem(user.userType));
        ui->userTable->setItem(i, COL_REGISTER_TIME, new QTableWidgetItem("N/A")); // 需要添加注册时间字段
    }
    
    // 调整列宽
    ui->userTable->resizeColumnsToContents();
}

// 航班管理功能
void AdminMainWindow::onAddFlight()
{
    showFlightDialog();
}

void AdminMainWindow::onEditFlight()
{
    int currentRow = ui->flightTable->currentRow();
    if (currentRow < 0 || currentRow >= currentFlights.size()) {
        QMessageBox::warning(this, "提示", "请先选择一个航班！");
        return;
    }
    
    const Flight& flight = currentFlights[currentRow];
    showFlightDialog(flight, true);
}

void AdminMainWindow::onDeleteFlight()
{
    int currentRow = ui->flightTable->currentRow();
    if (currentRow < 0 || currentRow >= currentFlights.size()) {
        QMessageBox::warning(this, "提示", "请先选择一个航班！");
        return;
    }
    
    const Flight& flight = currentFlights[currentRow];
    
    int reply = QMessageBox::question(this, "确认删除", 
        QString("确定要删除航班 %1 吗？此操作不可恢复！").arg(flight.flightNumber),
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteFlight(flight.flightNumber)) {
            QMessageBox::information(this, "成功", "航班删除成功！");
            loadFlights(); // 刷新列表
        } else {
            QMessageBox::critical(this, "错误", "航班删除失败！");
        }
    }
}

void AdminMainWindow::onRefreshFlights()
{
    loadFlights();
}

void AdminMainWindow::onSearchFlight()
{
    QString searchText = ui->flightSearchEdit->text().trimmed();
    if (searchText.isEmpty()) {
        displayFlights(currentFlights);
        return;
    }
    
    QVector<Flight> filteredFlights;
    for (const Flight& flight : currentFlights) {
        if (flight.flightNumber.contains(searchText, Qt::CaseInsensitive) ||
            flight.airline.contains(searchText, Qt::CaseInsensitive) ||
            flight.departureCity.contains(searchText, Qt::CaseInsensitive) ||
            flight.arrivalCity.contains(searchText, Qt::CaseInsensitive)) {
            filteredFlights.append(flight);
        }
    }
    
    displayFlights(filteredFlights);
    ui->statusbar->showMessage(QString("搜索到 %1 个航班").arg(filteredFlights.size()), 3000);
}

void AdminMainWindow::onFlightDoubleClicked(int row, int column)
{
    Q_UNUSED(column)
    
    if (row < 0 || row >= currentFlights.size()) {
        return;
    }
    
    const Flight& flight = currentFlights[row];
    showFlightDialog(flight, true);
}

void AdminMainWindow::showFlightDialog(const Flight& flight, bool isEdit)
{
    AddFlightDialog *dialog;
    
    if (isEdit) {
        // 使用编辑模式构造函数
        dialog = new AddFlightDialog(flight, this);
    } else {
        // 使用添加模式构造函数
        dialog = new AddFlightDialog(this);
    }
    
    if (dialog->exec() == QDialog::Accepted) {
        // 刷新航班列表
        loadFlights();
        if (isEdit) {
            QMessageBox::information(this, "提示", "航班信息更新成功！");
        } else {
            QMessageBox::information(this, "提示", "航班添加成功！");
        }
    }
    
    delete dialog;
}

// 其他方法的空实现，后续完善
void AdminMainWindow::onViewUser() {
    QMessageBox::information(this, "提示", "用户详情功能正在开发中...");
}

void AdminMainWindow::onResetPassword() {
    QMessageBox::information(this, "提示", "重置密码功能正在开发中...");
}

void AdminMainWindow::onRefreshUsers() {
    loadUsers();
}

void AdminMainWindow::onSearchUser() {
    QMessageBox::information(this, "提示", "用户搜索功能正在开发中...");
}

void AdminMainWindow::onUserDoubleClicked(int row, int column) {
    Q_UNUSED(row)
    Q_UNUSED(column)
    QMessageBox::information(this, "提示", "用户详情功能正在开发中...");
}

void AdminMainWindow::onGenerateStats() {
    QDate fromDate = ui->statsFromDate->date();
    QDate toDate = ui->statsToDate->date();
    
    if (fromDate > toDate) {
        QMessageBox::warning(this, "错误", "开始日期不能晚于结束日期！");
        return;
    }
    
    QString statsType = ui->statsTypeCombo->currentText();
    
    if (statsType == "总体统计") {
        generateOverallStats(fromDate, toDate);
    } else if (statsType == "航班统计") {
        generateFlightStats(fromDate, toDate);
    } else if (statsType == "用户统计") {
        generateUserStats(fromDate, toDate);
    } else if (statsType == "收入统计") {
        generateRevenueStats(fromDate, toDate);
    }
}

void AdminMainWindow::generateOverallStats(const QDate& fromDate, const QDate& toDate)
{
    int totalFlights = DatabaseManager::instance().getFlightCount(fromDate, toDate);
    int totalUsers = DatabaseManager::instance().getUserCount();
    int totalTickets = DatabaseManager::instance().getTicketCount(fromDate, toDate);
    double totalRevenue = DatabaseManager::instance().getTotalRevenue(fromDate, toDate);
    
    QString stats = QString(
        "总体统计报表\n"
        "统计时间: %1 至 %2\n\n"
        "航班总数: %3\n"
        "用户总数: %4\n"
        "售票总数: %5\n"
        "总收入: ¥%6\n"
        "平均票价: ¥%7\n\n"
        "生成时间: %8"
    ).arg(fromDate.toString("yyyy-MM-dd"))
     .arg(toDate.toString("yyyy-MM-dd"))
     .arg(totalFlights)
     .arg(totalUsers)
     .arg(totalTickets)
     .arg(totalRevenue, 0, 'f', 2)
     .arg(totalTickets > 0 ? totalRevenue / totalTickets : 0.0, 0, 'f', 2)
     .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    
    ui->statsTextEdit->setText(stats);
}

void AdminMainWindow::generateFlightStats(const QDate& fromDate, const QDate& toDate) {
    Q_UNUSED(fromDate)
    Q_UNUSED(toDate)
    ui->statsTextEdit->setText("航班统计功能正在开发中...");
}

void AdminMainWindow::generateUserStats(const QDate& fromDate, const QDate& toDate) {
    Q_UNUSED(fromDate)
    Q_UNUSED(toDate)
    ui->statsTextEdit->setText("用户统计功能正在开发中...");
}

void AdminMainWindow::generateRevenueStats(const QDate& fromDate, const QDate& toDate) {
    Q_UNUSED(fromDate)
    Q_UNUSED(toDate)
    ui->statsTextEdit->setText("收入统计功能正在开发中...");
}

void AdminMainWindow::onExportStats() {
    QMessageBox::information(this, "提示", "导出统计功能正在开发中...");
}

void AdminMainWindow::onBackupDatabase() {
    QMessageBox::information(this, "提示", "数据库备份功能正在开发中...");
}

void AdminMainWindow::onRestoreDatabase() {
    QMessageBox::information(this, "提示", "数据库恢复功能正在开发中...");
}

void AdminMainWindow::onOptimizeDatabase() {
    QMessageBox::information(this, "提示", "数据库优化功能正在开发中...");
}

void AdminMainWindow::onRefreshLog() {
    updateSystemLog();
}

void AdminMainWindow::onClearLog() {
    ui->logTextEdit->clear();
}

void AdminMainWindow::onExportLog() {
    QMessageBox::information(this, "提示", "导出日志功能正在开发中...");
}

void AdminMainWindow::updateSystemLog() {
    QString log = QString(
        "系统日志 - %1\n\n"
        "[INFO] 管理员 %2 登录系统\n"
        "[INFO] 加载了 %3 个航班数据\n"
        "[INFO] 加载了 %4 个用户数据\n"
        "[INFO] 系统运行正常\n"
    ).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
     .arg(currentUsername)
     .arg(currentFlights.size())
     .arg(currentUsers.size());
    
    ui->logTextEdit->setText(log);
}

void AdminMainWindow::onImportData() {
    QMessageBox::information(this, "提示", "导入数据功能正在开发中...");
}

void AdminMainWindow::onExportData() {
    QMessageBox::information(this, "提示", "导出数据功能正在开发中...");
}

void AdminMainWindow::onPreferences() {
    QMessageBox::information(this, "提示", "偏好设置功能正在开发中...");
}

void AdminMainWindow::onChangePassword() {
    QMessageBox::information(this, "提示", "修改密码功能正在开发中...");
}

void AdminMainWindow::onAbout() {
    QMessageBox::about(this, "关于", 
        "航班票务管理系统 v1.0\n\n"
        "管理员版本\n"
        "基于Qt和SQLite开发\n"
        "支持航班管理、用户管理、统计报表等功能");
}

void AdminMainWindow::onUserManual() {
    QMessageBox::information(this, "提示", "使用手册功能正在开发中...");
}

void AdminMainWindow::onExit() {
    int result = QMessageBox::question(this, "退出", "确定要退出管理系统吗？");
    if (result == QMessageBox::Yes) {
        close();
    }
}

void AdminMainWindow::on_addFlightBtn_clicked()
{
    onAddFlight();
}

void AdminMainWindow::on_editFlightBtn_clicked()
{
    onEditFlight();
}

void AdminMainWindow::on_deleteFlightBtn_clicked()
{
    onDeleteFlight();
}

