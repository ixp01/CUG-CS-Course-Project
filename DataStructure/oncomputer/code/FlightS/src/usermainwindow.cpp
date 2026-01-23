#include "usermainwindow.h"
#include "ui_user_main.h"
#include "../include/common.h"
#include <QDate>
#include <QHeaderView>
#include <QContextMenuEvent>
#include <QTableWidgetItem>
#include <QScreen>
#include <QAbstractItemView>

UserMainWindow::UserMainWindow(int userId, const QString& username, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::UserMainWindow)
    , currentUserId(userId)
    , currentUsername(username)
{
    ui->setupUi(this);
    setupUI();
    setupTableHeaders();
    loadCities();
    
    // 初始化图算法
    flightGraph.buildGraph(DatabaseManager::instance().getAllFlights());
    
    // 设置窗口标题
    setWindowTitle(QString("航班票务系统 - 用户: %1").arg(username));
}

UserMainWindow::~UserMainWindow()
{
    delete ui;
}

void UserMainWindow::setupUI()
{
    // 设置日期控件默认值
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setMinimumDate(QDate::currentDate());
    ui->dateEdit->setCalendarPopup(true);
    
    // 连接信号槽
    connect(ui->searchButton, &QPushButton::clicked, this, &UserMainWindow::onSearchFlights);
    connect(ui->transferButton, &QPushButton::clicked, this, &UserMainWindow::onTransferSearch);
    connect(ui->flightTable, &QTableWidget::cellDoubleClicked, this, &UserMainWindow::onFlightDoubleClicked);
    
    // 菜单操作
    connect(ui->actionProfile, &QAction::triggered, this, &UserMainWindow::onShowProfile);
    connect(ui->actionLogout, &QAction::triggered, this, &UserMainWindow::onLogout);
    connect(ui->actionAbout, &QAction::triggered, this, &UserMainWindow::onAbout);
    
    // 标签页切换时刷新数据
    connect(ui->tabWidget, &QTabWidget::currentChanged, 
            [this](int index) {
        switch (index) {
            case 1: // 我的机票
                refreshMyTickets();
                break;
            case 2: // 预约抢票
                refreshReservations();
                break;
        }
    });
}

void UserMainWindow::setupTableHeaders()
{
    // 航班表头 - 添加操作列
    QStringList flightHeaders = {"航班号", "航空公司", "出发地", "目的地", 
                                "起飞时间", "到达时间", "余票", "价格", "状态", "操作"};
    ui->flightTable->setColumnCount(flightHeaders.size());
    ui->flightTable->setHorizontalHeaderLabels(flightHeaders);
    ui->flightTable->horizontalHeader()->setStretchLastSection(true);
    ui->flightTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->flightTable->setAlternatingRowColors(true);
    
    // 机票表头
    QStringList ticketHeaders = {"票号", "航班号", "出发地", "目的地", 
                                "起飞时间", "座位号", "价格", "状态"};
    ui->ticketTable->setColumnCount(ticketHeaders.size());
    ui->ticketTable->setHorizontalHeaderLabels(ticketHeaders);
    ui->ticketTable->horizontalHeader()->setStretchLastSection(true);
    ui->ticketTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->ticketTable->setAlternatingRowColors(true);
    
    // 预约表头  
    QStringList reservationHeaders = {"预约号", "航班号", "出发地", "目的地", 
                                     "预约时间", "优先级"};
    ui->reservationTable->setColumnCount(reservationHeaders.size());
    ui->reservationTable->setHorizontalHeaderLabels(reservationHeaders);
    ui->reservationTable->horizontalHeader()->setStretchLastSection(true);
    ui->reservationTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->reservationTable->setAlternatingRowColors(true);
}

void UserMainWindow::loadCities()
{
    QVector<QString> cities = DatabaseManager::instance().getAllCities();
    
    // 添加"不限"选项
    ui->fromCombo->addItem("不限");
    ui->toCombo->addItem("不限");
    
    // 添加城市列表
    for (const QString& city : cities) {
        ui->fromCombo->addItem(city);
        ui->toCombo->addItem(city);
    }
}

void UserMainWindow::onSearchFlights()
{
    QString from = ui->fromCombo->currentText();
    QString to = ui->toCombo->currentText();
    QDate date = ui->dateEdit->date();
    
    // 验证查询条件
    if (from == to && from != "不限") {
        QMessageBox::warning(this, "查询错误", "出发地和目的地不能相同！");
        return;
    }
    
    // 执行查询
    ui->statusbar->showMessage("正在查询航班...");
    
    QVector<Flight> flights = DatabaseManager::instance().searchFlights(
        from == "不限" ? QString() : from,
        to == "不限" ? QString() : to,
        date
    );
    
    displayFlights(flights);
    
    ui->statusbar->showMessage(QString("找到 %1 个航班").arg(flights.size()), 3000);
}

void UserMainWindow::displayFlights(const QVector<Flight>& flights)
{
    currentFlights = flights;
    ui->flightTable->setRowCount(flights.size());
    
    for (int i = 0; i < flights.size(); ++i) {
        const Flight& flight = flights[i];
        
        ui->flightTable->setItem(i, COL_FLIGHT_NUMBER, new QTableWidgetItem(flight.flightNumber));
        ui->flightTable->setItem(i, COL_AIRLINE, new QTableWidgetItem(flight.airline));
        ui->flightTable->setItem(i, COL_DEPARTURE_CITY, new QTableWidgetItem(flight.departureCity));
        ui->flightTable->setItem(i, COL_ARRIVAL_CITY, new QTableWidgetItem(flight.arrivalCity));
        ui->flightTable->setItem(i, COL_DEPARTURE_TIME, 
                                new QTableWidgetItem(flight.departureTime.toString("MM-dd hh:mm")));
        ui->flightTable->setItem(i, COL_ARRIVAL_TIME, 
                                new QTableWidgetItem(flight.arrivalTime.toString("MM-dd hh:mm")));
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
        } else if (flight.availableSeats == 0) {
            statusItem->setBackground(QColor(108, 117, 125)); // 灰色
            statusItem->setForeground(QColor(255, 255, 255));
        }
        
        // 添加操作按钮
        QPushButton* actionButton = new QPushButton();
        actionButton->setProperty("flightIndex", i); // 存储航班索引
        
        if (flight.status == Constants::FLIGHT_CANCELLED) {
            actionButton->setText("已取消");
            actionButton->setEnabled(false);
            actionButton->setStyleSheet("background-color: #dc3545; color: white;");
        } else if (flight.availableSeats <= 0) {
            actionButton->setText("预约");
            actionButton->setStyleSheet("background-color: #ffc107; color: black;");
        } else {
            actionButton->setText("购买");
            actionButton->setStyleSheet("background-color: #28a745; color: white;");
        }
        
        // 连接按钮点击事件 - 使用flight对象避免索引问题
        connect(actionButton, &QPushButton::clicked, this, [this, flight]() {
            showFlightDetails(flight);
        });
        
        ui->flightTable->setCellWidget(i, COL_ACTION, actionButton);
    }
    
    // 调整列宽
    ui->flightTable->resizeColumnsToContents();
    
    // 设置操作列宽度固定
    ui->flightTable->setColumnWidth(COL_ACTION, 80);
}

void UserMainWindow::onFlightDoubleClicked(int row, int column)
{
    Q_UNUSED(column)
    
    if (row < 0 || row >= currentFlights.size()) {
        return;
    }
    
    const Flight& flight = currentFlights[row];
    showFlightDetails(flight);
}

void UserMainWindow::showFlightDetails(const Flight& flight)
{
    QString details = QString(
        "航班详情\n\n"
        "航班号: %1\n"
        "航空公司: %2\n"
        "出发地: %3\n"
        "目的地: %4\n"
        "起飞时间: %5\n"
        "到达时间: %6\n"
        "总座位: %7\n"
        "余票: %8\n"
        "价格: ¥%9\n"
        "状态: %10"
    ).arg(flight.flightNumber, flight.airline, flight.departureCity, flight.arrivalCity)
     .arg(flight.departureTime.toString("yyyy-MM-dd hh:mm"))
     .arg(flight.arrivalTime.toString("yyyy-MM-dd hh:mm"))
     .arg(flight.totalSeats).arg(flight.availableSeats)
     .arg(flight.price, 0, 'f', 0).arg(flight.status);
    
    if (!flight.stopovers.isEmpty()) {
        details += "\n经停: " + flight.stopovers.join(", ");
    }
    
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("航班详情");
    msgBox.setText(details);
    
    // 根据航班状态显示不同按钮
    if (flight.status == Constants::FLIGHT_CANCELLED) {
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setInformativeText("此航班已取消");
    } else if (flight.availableSeats <= 0) {
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.button(QMessageBox::Yes)->setText("加入预约队列");
        msgBox.button(QMessageBox::No)->setText("取消");
        msgBox.setInformativeText("此航班已无余票，是否加入预约队列？");
    } else {
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.button(QMessageBox::Yes)->setText("立即购买");
        msgBox.button(QMessageBox::No)->setText("取消");
        msgBox.setInformativeText("是否购买此航班机票？");
    }
    
    int result = msgBox.exec();
    if (result == QMessageBox::Yes) {
        if (flight.availableSeats <= 0) {
            // 加入预约队列
            bool success = DatabaseManager::instance().addToReservationQueue(
                currentUserId, flight.flightNumber, Constants::PRIORITY_NORMAL);
            
            if (success) {
                QMessageBox::information(this, "预约成功", 
                    QString("已成功加入航班 %1 的预约队列！\n"
                           "有座位释放时我们会自动为您购票。")
                    .arg(flight.flightNumber));
                refreshReservations(); // 刷新预约列表
            } else {
                QMessageBox::warning(this, "预约失败", 
                    "加入预约队列失败，您可能已在该航班的预约队列中。");
            }
        } else {
            // 立即购票
            TicketResult result = DatabaseManager::instance().bookTicket(currentUserId, flight.flightNumber);
            
            switch (result) {
                case TicketResult::Success:
                    QMessageBox::information(this, "购票成功", 
                        QString("恭喜！您已成功购买航班 %1 的机票！\n"
                               "请在'我的机票'中查看详情。")
                        .arg(flight.flightNumber));
                    // 刷新航班列表和机票列表
                    onSearchFlights();
                    refreshMyTickets();
                    break;
                case TicketResult::NoSeats:
                    QMessageBox::warning(this, "购票失败", "抱歉，该航班已无余票！");
                    break;
                case TicketResult::FlightCancelled:
                    QMessageBox::warning(this, "购票失败", "该航班已取消！");
                    break;
                case TicketResult::InvalidFlight:
                    QMessageBox::warning(this, "购票失败", "航班信息无效或不存在！");
                    break;
                case TicketResult::UserNotFound:
                    QMessageBox::critical(this, "购票失败", "用户信息错误，请重新登录！");
                    break;
                case TicketResult::AddedToQueue:
                    QMessageBox::information(this, "已加入预约", "已自动加入预约队列，有座位时将通知您！");
                    break;
                case TicketResult::Failed:
                default:
                    QMessageBox::critical(this, "购票失败", "购票过程中发生未知错误，请稍后重试。");
                    break;
            }
        }
    }
}

// 其他方法的空实现，后续完善
void UserMainWindow::onTransferSearch() {
    QMessageBox::information(this, "提示", "转机查询功能正在开发中...");
}

void UserMainWindow::refreshMyTickets() {
    // 获取用户机票
    QVector<Ticket> tickets = DatabaseManager::instance().getUserTickets(currentUserId);
    
    // 清空现有内容
    QWidget* ticketsTab = ui->tabWidget->widget(1);
    
    // 移除旧的表格
    QTableWidget* oldTable = ticketsTab->findChild<QTableWidget*>("ticketsTable");
    if (oldTable) {
        delete oldTable;
    }
    
    // 创建新的机票表格
    QTableWidget* table = new QTableWidget(ticketsTab);
    table->setObjectName("ticketsTable");
    
    // 设置表格列
    table->setColumnCount(7);
    QStringList headers = {"航班号", "出发地", "目的地", "座位号", "价格", "状态", "操作"};
    table->setHorizontalHeaderLabels(headers);
    
    // 设置行数
    table->setRowCount(tickets.size());
    
    // 填充数据
    for (int i = 0; i < tickets.size(); ++i) {
        const Ticket& ticket = tickets[i];
        
        // 获取航班详细信息
        Flight flight = DatabaseManager::instance().getFlight(ticket.flightNumber);
        QVector<Flight> flightInfo;
        if (flight.isValid()) {
            flightInfo.append(flight);
        }
        
        QString departureCity = "未知";
        QString arrivalCity = "未知";
        if (!flightInfo.isEmpty()) {
            departureCity = flightInfo[0].departureCity;
            arrivalCity = flightInfo[0].arrivalCity;
        }
        
        table->setItem(i, 0, new QTableWidgetItem(ticket.flightNumber));
        table->setItem(i, 1, new QTableWidgetItem(departureCity));
        table->setItem(i, 2, new QTableWidgetItem(arrivalCity));
        table->setItem(i, 3, new QTableWidgetItem(ticket.seatNumber));
        table->setItem(i, 4, new QTableWidgetItem(QString("¥%1").arg(ticket.price, 0, 'f', 2)));
        
        // 状态显示
        QTableWidgetItem* statusItem = new QTableWidgetItem();
        if (ticket.status == Constants::TICKET_BOOKED) {
            statusItem->setText("已预订");
            statusItem->setBackground(QBrush(QColor(144, 238, 144))); // 浅绿色
        } else if (ticket.status == Constants::TICKET_CANCELLED) {
            statusItem->setText("已取消");
            statusItem->setBackground(QBrush(QColor(255, 182, 193))); // 浅红色
        }
        table->setItem(i, 5, statusItem);
        
        // 操作按钮
        QPushButton* refundBtn = new QPushButton("退票");
        refundBtn->setProperty("ticketId", ticket.ticketId);
        refundBtn->setEnabled(ticket.status == Constants::TICKET_BOOKED);
        
        connect(refundBtn, &QPushButton::clicked, this, [this, ticket]() {
            onRefundTicket(ticket.ticketId);
        });
        
        table->setCellWidget(i, 6, refundBtn);
    }
    
    // 设置表格属性
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setAlternatingRowColors(true);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 设置布局
    QVBoxLayout* layout = new QVBoxLayout(ticketsTab);
    
    QLabel* titleLabel = new QLabel("我的机票", ticketsTab);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin: 10px;");
    
    layout->addWidget(titleLabel);
    layout->addWidget(table);
    
    ticketsTab->setLayout(layout);
    
    qDebug() << QString("刷新机票列表完成，共%1张机票").arg(tickets.size());
}

void UserMainWindow::onRefundTicket(int ticketId) {
    int reply = QMessageBox::question(this, "确认退票", 
        "确定要退票吗？退票后将不可恢复。",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        bool success = DatabaseManager::instance().refundTicket(ticketId);
        
        if (success) {
            QMessageBox::information(this, "退票成功", "您的机票已成功退票！");
            refreshMyTickets(); // 刷新机票列表
        } else {
            QMessageBox::critical(this, "退票失败", "退票过程中发生错误，请稍后重试。");
        }
    }
}

void UserMainWindow::refreshReservations() {
    // 获取用户预约
    QVector<Reservation> reservations = DatabaseManager::instance().getUserReservations(currentUserId);
    
    // 清空现有内容
    QWidget* reservationsTab = ui->tabWidget->widget(2);
    
    // 移除旧的表格
    QTableWidget* oldTable = reservationsTab->findChild<QTableWidget*>("reservationsTable");
    if (oldTable) {
        delete oldTable;
    }
    
    // 创建新的预约表格
    QTableWidget* table = new QTableWidget(reservationsTab);
    table->setObjectName("reservationsTable");
    
    // 设置表格列
    table->setColumnCount(6);
    QStringList headers = {"航班号", "出发地", "目的地", "请求时间", "优先级", "操作"};
    table->setHorizontalHeaderLabels(headers);
    
    // 设置行数
    table->setRowCount(reservations.size());
    
    // 填充数据
    for (int i = 0; i < reservations.size(); ++i) {
        const Reservation& reservation = reservations[i];
        
        // 获取航班详细信息
        Flight flight = DatabaseManager::instance().getFlight(reservation.flightNumber);
        QVector<Flight> flightInfo;
        if (flight.isValid()) {
            flightInfo.append(flight);
        }
        
        QString departureCity = "未知";
        QString arrivalCity = "未知";
        if (!flightInfo.isEmpty()) {
            departureCity = flightInfo[0].departureCity;
            arrivalCity = flightInfo[0].arrivalCity;
        }
        
        table->setItem(i, 0, new QTableWidgetItem(reservation.flightNumber));
        table->setItem(i, 1, new QTableWidgetItem(departureCity));
        table->setItem(i, 2, new QTableWidgetItem(arrivalCity));
        table->setItem(i, 3, new QTableWidgetItem(
            reservation.requestTime.toString("yyyy-MM-dd hh:mm")));
        
        // 优先级显示
        QString priorityText;
        switch (reservation.priority) {
            case Constants::PRIORITY_URGENT:
                priorityText = "紧急";
                break;
            case Constants::PRIORITY_VIP:
                priorityText = "VIP";
                break;
            case Constants::PRIORITY_NORMAL:
                priorityText = "普通";
                break;
            default:
                priorityText = QString::number(reservation.priority);
        }
        table->setItem(i, 4, new QTableWidgetItem(priorityText));
        
        // 操作按钮
        QPushButton* cancelBtn = new QPushButton("取消预约");
        cancelBtn->setProperty("reservationId", reservation.reservationId);
        
        connect(cancelBtn, &QPushButton::clicked, this, [this, reservation]() {
            onCancelReservation(reservation.reservationId);
        });
        
        table->setCellWidget(i, 5, cancelBtn);
    }
    
    // 设置表格属性
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setAlternatingRowColors(true);
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // 设置布局
    QVBoxLayout* layout = new QVBoxLayout(reservationsTab);
    
    QLabel* titleLabel = new QLabel("我的预约", reservationsTab);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin: 10px;");
    
    layout->addWidget(titleLabel);
    layout->addWidget(table);
    
    reservationsTab->setLayout(layout);
    
    qDebug() << QString("刷新预约列表完成，共%1个预约").arg(reservations.size());
}

void UserMainWindow::onCancelReservation(int reservationId) {
    int reply = QMessageBox::question(this, "确认取消", 
        "确定要取消此预约吗？",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        bool success = DatabaseManager::instance().removeFromReservationQueue(reservationId);
        
        if (success) {
            QMessageBox::information(this, "取消成功", "预约已成功取消！");
            refreshReservations(); // 刷新预约列表
        } else {
            QMessageBox::critical(this, "取消失败", "取消预约过程中发生错误，请稍后重试。");
        }
    }
}

void UserMainWindow::displayTickets() {
    refreshMyTickets(); // 调用已实现的刷新机票列表方法
}

void UserMainWindow::displayReservations() {
    refreshReservations(); // 调用已实现的刷新预约列表方法
}

bool UserMainWindow::confirmBooking(const Flight& flight) {
    // 显示购票确认对话框
    QString confirmText = QString(
        "确认购买以下航班的机票：\n\n"
        "航班号: %1\n"
        "航空公司: %2\n"
        "出发: %3 → %4\n"
        "起飞时间: %5\n"
        "到达时间: %6\n"
        "票价: ¥%7\n"
        "余票: %8 张\n\n"
        "确定要购买吗？"
    ).arg(flight.flightNumber)
     .arg(flight.airline)
     .arg(flight.departureCity, flight.arrivalCity)
     .arg(flight.departureTime.toString("yyyy-MM-dd hh:mm"))
     .arg(flight.arrivalTime.toString("yyyy-MM-dd hh:mm"))
     .arg(flight.price, 0, 'f', 0)
     .arg(flight.availableSeats);
    
    int reply = QMessageBox::question(this, "确认购票", confirmText,
                                     QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // 尝试购票
        TicketResult result = DatabaseManager::instance().bookTicket(currentUserId, flight.flightNumber);
        
        switch (result) {
            case TicketResult::Success:
                QMessageBox::information(this, "购票成功", "恭喜您！机票购买成功。");
                onSearchFlights(); // 刷新航班列表
                refreshMyTickets(); // 刷新我的机票
                return true;
                
            case TicketResult::NoSeats:
                {
                    int addToQueue = QMessageBox::question(this, "座位已满", 
                        "很抱歉，该航班座位已满。是否加入预约队列？当有座位时会自动为您购票。",
                        QMessageBox::Yes | QMessageBox::No);
                    
                    if (addToQueue == QMessageBox::Yes) {
                        bool success = DatabaseManager::instance().addToReservationQueue(
                            currentUserId, flight.flightNumber, Constants::PRIORITY_NORMAL);
                        
                        if (success) {
                            QMessageBox::information(this, "预约成功", "已加入预约队列，有座位时会自动通知您。");
                            refreshReservations(); // 刷新预约列表
                        } else {
                            QMessageBox::critical(this, "预约失败", "加入预约队列失败，请稍后重试。");
                        }
                    }
                }
                break;
                
            case TicketResult::AddedToQueue:
                QMessageBox::information(this, "加入队列", "座位已满，已自动为您加入预约队列。");
                refreshReservations();
                break;
                
            case TicketResult::FlightCancelled:
                QMessageBox::warning(this, "航班取消", "很抱歉，该航班已被取消。");
                break;
                
            case TicketResult::InvalidFlight:
                QMessageBox::critical(this, "航班无效", "航班信息无效，请重新选择。");
                break;
                
            case TicketResult::UserNotFound:
                QMessageBox::critical(this, "用户错误", "用户信息错误，请重新登录。");
                break;
                
            default:
                QMessageBox::critical(this, "购票失败", "购票过程中发生未知错误，请稍后重试。");
                break;
        }
    }
    
    return false;
}

void UserMainWindow::onShowProfile() {
    User user = DatabaseManager::instance().getUserInfo(currentUserId);
    QString info = QString(
        "用户信息\n\n"
        "用户ID: %1\n"
        "用户名: %2\n"
        "邮箱: %3\n"
        "电话: %4\n"
        "用户类型: %5"
    ).arg(user.userId).arg(user.username).arg(user.email).arg(user.phone).arg(user.userType);
    
    QMessageBox::information(this, "个人信息", info);
}

void UserMainWindow::onLogout() {
    int result = QMessageBox::question(this, "退出登录", "确定要退出登录吗？");
    if (result == QMessageBox::Yes) {
        close();
    }
}

void UserMainWindow::onAbout() {
    QMessageBox::about(this, "关于", 
        "航班票务管理系统 v1.0\n\n"
        "基于Qt和SQLite开发\n"
        "支持航班查询、购票、转机推荐等功能");
} 