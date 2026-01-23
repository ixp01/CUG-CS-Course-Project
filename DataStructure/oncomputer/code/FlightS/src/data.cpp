#include "data.h"
#include <QCryptographicHash>
#include <QRegularExpression>
#include <QSqlRecord>
#include <QVariant>
#include <QDir>
#include <QCoreApplication>
#include <QSet>

// 静态成员初始化
QMutex DatabaseManager::dbMutex;

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

bool DatabaseManager::connectToDatabase(const QString& dbPath) {
    QMutexLocker locker(&dbMutex);
    
    // 如果已经连接，先关闭
    if (db.isOpen()) {
        db.close();
    }
    
    // 创建数据库连接
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    
    if (!db.open()) {
        qDebug() << "数据库连接失败:" << db.lastError().text();
        return false;
    }
    
    qDebug() << "数据库连接成功:" << dbPath;
    return true;
}

void DatabaseManager::closeDatabase() {
    QMutexLocker locker(&dbMutex);
    if (db.isOpen()) {
        db.close();
        qDebug() << "数据库连接已关闭";
    }
}



bool DatabaseManager::executeQuery(QSqlQuery& query, const QString& errorMsg) {
    if (!query.exec()) {
        qDebug() << errorMsg << query.lastError().text();
        return false;
    }
    return true;
}

QString DatabaseManager::hashPassword(const QString& password) {
    // 改为明文存储，直接返回原密码
    return password;
}

bool DatabaseManager::validateEmail(const QString& email) {
    QRegularExpression regex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    return regex.match(email).hasMatch();
}

bool DatabaseManager::validatePhone(const QString& phone) {
    QRegularExpression regex(R"(^1[3-9]\d{9}$)");
    return regex.match(phone).hasMatch();
}

// 用户相关操作实现
bool DatabaseManager::registerUser(const QString& username, const QString& password, 
                                  const QString& email, const QString& phone, 
                                  const QString& userType) {
    QMutexLocker locker(&dbMutex);
    
    // 验证输入
    if (username.isEmpty() || password.isEmpty()) {
        qDebug() << "用户名或密码不能为空";
        return false;
    }
    
    if (!email.isEmpty() && !validateEmail(email)) {
        qDebug() << "邮箱格式不正确";
        return false;
    }
    
    if (!phone.isEmpty() && !validatePhone(phone)) {
        qDebug() << "手机号格式不正确";
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare("INSERT INTO user (username, password, email, phone, user_type) "
                  "VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(password); // 直接使用明文密码
    query.addBindValue(email);
    query.addBindValue(phone);
    query.addBindValue(userType);
    
    if (!executeQuery(query, "用户注册失败:")) {
        return false;
    }
    
    qDebug() << "用户注册成功:" << username;
    return true;
}

int DatabaseManager::authenticateUser(const QString& username, const QString& password) {
    QMutexLocker locker(&dbMutex);
    
    qDebug() << "尝试用户登录:" << username << "密码:" << password;
    
    // 检查数据库连接
    if (!db.isOpen()) {
        qDebug() << "数据库未连接";
        return -1;
    }
    
    // 直接构造SQL查询，避免参数绑定问题
    QString escapedUsername = username;
    QString escapedPassword = password;
    escapedUsername.replace("'", "''");  // 防止SQL注入，转义单引号
    escapedPassword.replace("'", "''");
    
    QString sql = QString("SELECT user_id FROM user WHERE username = '%1' AND password = '%2'")
                    .arg(escapedUsername)
                    .arg(escapedPassword);
                    
    qDebug() << "执行SQL:" << sql;
    
    QSqlQuery query(db);
    
    if (query.exec(sql)) {
        qDebug() << "SQL执行成功，查找匹配记录...";
        if (query.next()) {
            int userId = query.value("user_id").toInt();
            qDebug() << "用户认证成功:" << username << "ID:" << userId;
            return userId;
        } else {
            qDebug() << "未找到匹配的用户记录";
        }
    } else {
        qDebug() << "用户认证失败:" << username << "错误:" << query.lastError().text();
    }
    
    return -1;
}

User DatabaseManager::getUserInfo(int userId) {
    QMutexLocker locker(&dbMutex);
    
    User user;
    QSqlQuery query(db);
    query.prepare("SELECT user_id, username, password, email, phone, user_type "
                  "FROM user WHERE user_id = ?");
    query.addBindValue(userId);
    
    if (query.exec() && query.next()) {
        user.userId = query.value("user_id").toInt();
        user.username = query.value("username").toString();
        user.password = query.value("password").toString();
        user.email = query.value("email").toString();
        user.phone = query.value("phone").toString();
        user.userType = query.value("user_type").toString();
    } else {
        qDebug() << "获取用户信息失败:" << query.lastError().text();
    }
    
    return user;
}

bool DatabaseManager::updateUserInfo(const User& user) {
    QMutexLocker locker(&dbMutex);
    
    QSqlQuery query(db);
    query.prepare("UPDATE user SET username = ?, email = ?, phone = ?, user_type = ? "
                  "WHERE user_id = ?");
    query.addBindValue(user.username);
    query.addBindValue(user.email);
    query.addBindValue(user.phone);
    query.addBindValue(user.userType);
    query.addBindValue(user.userId);
    
    return executeQuery(query, "更新用户信息失败:");
}

// 管理员相关操作实现
int DatabaseManager::authenticateManager(const QString& username, const QString& password) {
    QMutexLocker locker(&dbMutex);
    
    qDebug() << "尝试管理员登录:" << username << "密码:" << password;
    
    // 检查数据库连接
    if (!db.isOpen()) {
        qDebug() << "数据库未连接";
        return -1;
    }
    
    // 直接构造SQL查询，避免参数绑定问题
    QString escapedUsername = username;
    QString escapedPassword = password;
    escapedUsername.replace("'", "''");  // 防止SQL注入，转义单引号
    escapedPassword.replace("'", "''");
    
    QString sql = QString("SELECT manager_id FROM manager WHERE username = '%1' AND password = '%2'")
                    .arg(escapedUsername)
                    .arg(escapedPassword);
                    
    qDebug() << "执行SQL:" << sql;
    
    QSqlQuery query(db);
    
    if (query.exec(sql)) {
        qDebug() << "SQL执行成功，查找匹配记录...";
        if (query.next()) {
            int managerId = query.value("manager_id").toInt();
            qDebug() << "管理员认证成功:" << username << "ID:" << managerId;
            return managerId;
        } else {
            qDebug() << "未找到匹配的管理员记录";
        }
    } else {
        qDebug() << "管理员认证失败:" << username << "错误:" << query.lastError().text();
    }
    
    return -1;
}

Manager DatabaseManager::getManagerInfo(int managerId) {
    QMutexLocker locker(&dbMutex);
    
    Manager manager;
    QSqlQuery query(db);
    query.prepare("SELECT manager_id, username, password FROM manager WHERE manager_id = ?");
    query.addBindValue(managerId);
    
    if (executeQuery(query, "获取管理员信息失败:") && query.next()) {
        manager.managerId = query.value(0).toInt();
        manager.username = query.value(1).toString();
        manager.password = query.value(2).toString();
    }
    
    return manager;
}

// 航班相关操作实现
QVector<Flight> DatabaseManager::getAllFlights() {
    QMutexLocker locker(&dbMutex);
    QVector<Flight> flights;
    
    QSqlQuery query(db);
    // 先查询表结构以确认字段名
    if (query.exec("PRAGMA table_info(flight)")) {
        qDebug() << "flight表结构:";
        while (query.next()) {
            qDebug() << "字段:" << query.value(1).toString() << "类型:" << query.value(2).toString();
        }
    }
    
    // 使用基本的SELECT查询，让数据库告诉我们实际有哪些字段
    if (query.exec("SELECT * FROM flight LIMIT 1")) {
        QSqlRecord record = query.record();
        qDebug() << "flight表字段数量:" << record.count();
        for (int i = 0; i < record.count(); ++i) {
            qDebug() << "字段" << i << ":" << record.fieldName(i);
        }
    }
    
    // 现在尝试查询所有航班数据
    if (query.exec("SELECT * FROM flight ORDER BY departure_time")) {
        while (query.next()) {
            Flight flight;
            QSqlRecord record = query.record();
            
            // 安全地获取字段值
            flight.flightNumber = query.value("flight_number").toString();
            flight.airline = query.value("airline").toString();
            flight.departureCity = query.value("departure_city").toString();
            flight.arrivalCity = query.value("arrival_city").toString();
            flight.departureTime = query.value("departure_time").toDateTime();
            flight.arrivalTime = query.value("arrival_time").toDateTime();
            
            QString stopover = query.value("stopover").toString();
            if (!stopover.isEmpty()) {
                flight.stopovers = stopover.split(",");
            }
            
            flight.totalSeats = query.value("total_seats").toInt();
            flight.availableSeats = query.value("available_seats").toInt();
            flight.price = query.value("price").toDouble();
            flight.status = query.value("status").toString();
            
            flights.append(flight);
        }
    } else {
        qDebug() << "查询flight表失败:" << query.lastError().text();
    }
    
    qDebug() << "获取航班数据:" << flights.size() << "条记录";
    return flights;
}

QVector<Flight> DatabaseManager::searchFlights(const QString& from, const QString& to, const QDate& date) {
    QMutexLocker locker(&dbMutex);
    QVector<Flight> flights;
    
    QSqlQuery query(db);
    QString sql = "SELECT * FROM flight WHERE 1=1";
    
    QStringList conditions;
    QVariantList values;
    
    if (!from.isEmpty() && from != "不限") {
        conditions << "departure_city = ?";
        values << from;
    }
    
    if (!to.isEmpty() && to != "不限") {
        conditions << "arrival_city = ?";
        values << to;
    }
    
    if (date.isValid()) {
        conditions << "DATE(departure_time) = ?";
        values << date.toString("yyyy-MM-dd");
    }
    
    if (!conditions.isEmpty()) {
        sql += " AND " + conditions.join(" AND ");
    }
    
    sql += " ORDER BY departure_time";
    
    query.prepare(sql);
    for (const QVariant& value : values) {
        query.addBindValue(value);
    }
    
    if (query.exec()) {
        while (query.next()) {
            Flight flight;
            flight.flightNumber = query.value("flight_number").toString();
            flight.airline = query.value("airline").toString();
            flight.departureCity = query.value("departure_city").toString();
            flight.arrivalCity = query.value("arrival_city").toString();
            flight.departureTime = query.value("departure_time").toDateTime();
            flight.arrivalTime = query.value("arrival_time").toDateTime();
            
            QString stopover = query.value("stopover").toString();
            if (!stopover.isEmpty()) {
                flight.stopovers = stopover.split(",");
            }
            
            flight.totalSeats = query.value("total_seats").toInt();
            flight.availableSeats = query.value("available_seats").toInt();
            flight.price = query.value("price").toDouble();
            flight.status = query.value("status").toString();
            
            flights.append(flight);
        }
    } else {
        qDebug() << "航班搜索失败:" << query.lastError().text();
    }
    
    qDebug() << QString("搜索航班: %1->%2 日期:%3, 找到%4条记录")
                .arg(from, to, date.toString(), QString::number(flights.size()));
    return flights;
}

Flight DatabaseManager::getFlight(const QString& flightNumber) {
    QMutexLocker locker(&dbMutex);
    Flight flight;
    
    if (flightNumber.isEmpty()) {
        qDebug() << "航班号为空";
        return flight;
    }
    
    QSqlQuery query(db);
    query.prepare("SELECT * FROM flight WHERE flight_number = ?");
    query.addBindValue(flightNumber);
    
    if (query.exec() && query.next()) {
        flight.flightNumber = query.value("flight_number").toString();
        flight.airline = query.value("airline").toString();
        flight.departureCity = query.value("departure_city").toString();
        flight.arrivalCity = query.value("arrival_city").toString();
        flight.departureTime = query.value("departure_time").toDateTime();
        flight.arrivalTime = query.value("arrival_time").toDateTime();
        
        QString stopover = query.value("stopover").toString();
        if (!stopover.isEmpty()) {
            flight.stopovers = stopover.split(",");
        }
        
        flight.totalSeats = query.value("total_seats").toInt();
        flight.availableSeats = query.value("available_seats").toInt();
        flight.price = query.value("price").toDouble();
        flight.status = query.value("status").toString();
    } else {
        qDebug() << "获取航班信息失败:" << flightNumber << query.lastError().text();
    }
    
    return flight;
}

bool DatabaseManager::addFlight(const Flight& flight) {
    QMutexLocker locker(&dbMutex);
    
    // 验证航班信息
    if (!flight.isValid()) {
        qDebug() << "航班信息无效";
        return false;
    }
    
    // 检查航班号是否已存在
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM flight WHERE flight_number = ?");
    checkQuery.addBindValue(flight.flightNumber);
    
    if (checkQuery.exec() && checkQuery.next()) {
        if (checkQuery.value(0).toInt() > 0) {
            qDebug() << "航班号已存在:" << flight.flightNumber;
            return false;
        }
    } else {
        qDebug() << "检查航班号失败:" << checkQuery.lastError().text();
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare("INSERT INTO flight (flight_number, airline, departure_city, arrival_city, "
                  "departure_time, arrival_time, total_seats, available_seats, status, price, stopover) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    
    query.addBindValue(flight.flightNumber);
    query.addBindValue(flight.airline);
    query.addBindValue(flight.departureCity);
    query.addBindValue(flight.arrivalCity);
    query.addBindValue(flight.departureTime);
    query.addBindValue(flight.arrivalTime);
    query.addBindValue(flight.totalSeats);
    query.addBindValue(flight.availableSeats);
    query.addBindValue(flight.status);
    query.addBindValue(flight.price);
    query.addBindValue(flight.stopovers.join(","));
    
    if (query.exec()) {
        qDebug() << "航班添加成功:" << flight.flightNumber;
        return true;
    } else {
        qDebug() << "航班添加失败:" << query.lastError().text();
        return false;
    }
}

bool DatabaseManager::updateFlight(const Flight& flight) {
    QMutexLocker locker(&dbMutex);
    
    // 验证航班信息
    if (!flight.isValid()) {
        qDebug() << "航班信息无效";
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare("UPDATE flight SET airline = ?, departure_city = ?, arrival_city = ?, "
                  "departure_time = ?, arrival_time = ?, total_seats = ?, available_seats = ?, "
                  "status = ?, price = ?, stopover = ? WHERE flight_number = ?");
    
    query.addBindValue(flight.airline);
    query.addBindValue(flight.departureCity);
    query.addBindValue(flight.arrivalCity);
    query.addBindValue(flight.departureTime);
    query.addBindValue(flight.arrivalTime);
    query.addBindValue(flight.totalSeats);
    query.addBindValue(flight.availableSeats);
    query.addBindValue(flight.status);
    query.addBindValue(flight.price);
    query.addBindValue(flight.stopovers.join(","));
    query.addBindValue(flight.flightNumber);
    
    if (query.exec()) {
        qDebug() << "航班更新成功:" << flight.flightNumber;
        return true;
    } else {
        qDebug() << "航班更新失败:" << query.lastError().text();
        return false;
    }
}

bool DatabaseManager::deleteFlight(const QString& flightNumber) {
    QMutexLocker locker(&dbMutex);
    
    if (flightNumber.isEmpty()) {
        qDebug() << "航班号为空";
        return false;
    }
    
    // 检查是否有已预订的机票
    QSqlQuery checkQuery(db);
    checkQuery.prepare("SELECT COUNT(*) FROM ticket WHERE flight_number = ? AND status = ?");
    checkQuery.addBindValue(flightNumber);
    checkQuery.addBindValue(Constants::TICKET_BOOKED);
    
    if (checkQuery.exec() && checkQuery.next()) {
        if (checkQuery.value(0).toInt() > 0) {
            qDebug() << "无法删除航班，存在已预订的机票:" << flightNumber;
            return false;
        }
    }
    
    // 先删除相关的预约记录
    QSqlQuery deleteReservations(db);
    deleteReservations.prepare("DELETE FROM reservation_queue WHERE flight_number = ?");
    deleteReservations.addBindValue(flightNumber);
    deleteReservations.exec();
    
    // 删除相关的已取消机票
    QSqlQuery deleteTickets(db);
    deleteTickets.prepare("DELETE FROM ticket WHERE flight_number = ? AND status = ?");
    deleteTickets.addBindValue(flightNumber);
    deleteTickets.addBindValue(Constants::TICKET_CANCELLED);
    deleteTickets.exec();
    
    // 删除航班
    QSqlQuery query(db);
    query.prepare("DELETE FROM flight WHERE flight_number = ?");
    query.addBindValue(flightNumber);
    
    if (query.exec()) {
        qDebug() << "航班删除成功:" << flightNumber;
        return true;
    } else {
        qDebug() << "航班删除失败:" << query.lastError().text();
        return false;
    }
}

bool DatabaseManager::updateFlightStatus(const QString& flightNumber, const QString& status) {
    QMutexLocker locker(&dbMutex);
    
    if (flightNumber.isEmpty() || status.isEmpty()) {
        qDebug() << "航班号或状态为空";
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare("UPDATE flight SET status = ? WHERE flight_number = ?");
    query.addBindValue(status);
    query.addBindValue(flightNumber);
    
    if (query.exec()) {
        qDebug() << "航班状态更新成功:" << flightNumber << "状态:" << status;
        return true;
    } else {
        qDebug() << "航班状态更新失败:" << query.lastError().text();
        return false;
    }
}

bool DatabaseManager::updateFlightTime(const QString& flightNumber, 
                                     const QDateTime& newDep, const QDateTime& newArr) {
    QMutexLocker locker(&dbMutex);
    
    if (flightNumber.isEmpty() || !newDep.isValid() || !newArr.isValid()) {
        qDebug() << "参数无效";
        return false;
    }
    
    if (newDep >= newArr) {
        qDebug() << "到达时间必须晚于出发时间";
        return false;
    }
    
    QSqlQuery query(db);
    query.prepare("UPDATE flight SET departure_time = ?, arrival_time = ? WHERE flight_number = ?");
    query.addBindValue(newDep);
    query.addBindValue(newArr);
    query.addBindValue(flightNumber);
    
    if (query.exec()) {
        qDebug() << "航班时间更新成功:" << flightNumber;
        return true;
    } else {
        qDebug() << "航班时间更新失败:" << query.lastError().text();
        return false;
    }
}

// 票务相关操作实现
TicketResult DatabaseManager::bookTicket(int userId, const QString& flightNumber) {
    QMutexLocker locker(&dbMutex);
    
    // 开始事务
    if (!db.transaction()) {
        qDebug() << "开始事务失败:" << db.lastError().text();
        return TicketResult::Failed;
    }
    
    QSqlQuery query(db);
    
    // 1. 检查航班是否存在和可用
    query.prepare("SELECT available_seats, status, price FROM flight WHERE flight_number = ?");
    query.addBindValue(flightNumber);
    
    if (!query.exec()) {
        db.rollback();
        qDebug() << "航班查询执行失败:" << query.lastError().text();
        return TicketResult::Failed;
    }
    if (!query.next()) {
        db.rollback();
        qDebug() << "航班不存在:" << flightNumber;
        return TicketResult::InvalidFlight;
    }
    
    int availableSeats = query.value("available_seats").toInt();
    QString status = query.value("status").toString();
    double price = query.value("price").toDouble();
    
    // 检查航班状态
    if (status == Constants::FLIGHT_CANCELLED) {
        db.rollback();
        return TicketResult::FlightCancelled;
    }
    
    // 检查余票
    if (availableSeats <= 0) {
        db.rollback();
        return TicketResult::NoSeats;
    }
    
    // 2. 检查用户是否存在
    query.prepare("SELECT user_id FROM user WHERE user_id = ?");
    query.addBindValue(userId);
    if (!query.exec()) {
        db.rollback();
        qDebug() << "用户查询执行失败:" << query.lastError().text();
        return TicketResult::Failed;
    }
    if (!query.next()) {
        db.rollback();
        qDebug() << "用户不存在:" << userId;
        return TicketResult::UserNotFound;
    }
    qDebug() << "用户验证通过:" << userId;
    
    // 3. 减少可用座位
    query.prepare("UPDATE flight SET available_seats = available_seats - 1 WHERE flight_number = ?");
    query.addBindValue(flightNumber);
    if (!query.exec()) {
        db.rollback();
        qDebug() << "更新座位失败:" << query.lastError().text();
        return TicketResult::Failed;
    }
    
    // 4. 生成座位号
    QString seatNumber = generateSeatNumber(flightNumber);
    
    // 5. 创建票务记录
    query.prepare("INSERT INTO ticket (flight_number, user_id, status, price, seat_number, booking_time) "
                  "VALUES (?, ?, ?, ?, ?, datetime('now'))");
    query.addBindValue(flightNumber);
    query.addBindValue(userId);
    query.addBindValue(Constants::TICKET_BOOKED);
    query.addBindValue(price);
    query.addBindValue(seatNumber);
    
    if (!query.exec()) {
        db.rollback();
        qDebug() << "创建票务记录失败:" << query.lastError().text();
        return TicketResult::Failed;
    }
    
    // 提交事务
    if (!db.commit()) {
        qDebug() << "提交事务失败:" << db.lastError().text();
        return TicketResult::Failed;
    }
    
    qDebug() << QString("购票成功: 用户%1, 航班%2, 座位%3")
                .arg(userId).arg(flightNumber, seatNumber);
    
    // 处理预约队列
    processReservationQueue(flightNumber);
    
    return TicketResult::Success;
}

bool DatabaseManager::refundTicket(int ticketId) {
    QMutexLocker locker(&dbMutex);
    
    // 开始事务
    if (!db.transaction()) {
        qDebug() << "开始退票事务失败:" << db.lastError().text();
        return false;
    }
    
    QSqlQuery query(db);
    
    // 1. 获取票务信息
    query.prepare("SELECT flight_number, status FROM ticket WHERE ticket_id = ?");
    query.addBindValue(ticketId);
    
    if (!query.exec() || !query.next()) {
        db.rollback();
        qDebug() << "票务不存在:" << ticketId;
        return false;
    }
    
    QString flightNumber = query.value("flight_number").toString();
    QString currentStatus = query.value("status").toString();
    
    // 检查是否已经是取消状态
    if (currentStatus == Constants::TICKET_CANCELLED) {
        db.rollback();
        qDebug() << "票务已经是取消状态:" << ticketId;
        return false;
    }
    
    // 2. 更新票务状态为取消
    query.prepare("UPDATE ticket SET status = ? WHERE ticket_id = ?");
    query.addBindValue(Constants::TICKET_CANCELLED);
    query.addBindValue(ticketId);
    
    if (!query.exec()) {
        db.rollback();
        qDebug() << "更新票务状态失败:" << query.lastError().text();
        return false;
    }
    
    // 3. 增加航班可用座位
    query.prepare("UPDATE flight SET available_seats = available_seats + 1 WHERE flight_number = ?");
    query.addBindValue(flightNumber);
    
    if (!query.exec()) {
        db.rollback();
        qDebug() << "更新航班座位失败:" << query.lastError().text();
        return false;
    }
    
    // 提交事务
    if (!db.commit()) {
        qDebug() << "提交退票事务失败:" << db.lastError().text();
        return false;
    }
    
    qDebug() << QString("退票成功: 票号%1, 航班%2").arg(ticketId).arg(flightNumber);
    
    // 处理预约队列
    processReservationQueue(flightNumber);
    
    return true;
}

QVector<Ticket> DatabaseManager::getUserTickets(int userId) {
    QMutexLocker locker(&dbMutex);
    QVector<Ticket> tickets;
    
    QSqlQuery query(db);
    query.prepare("SELECT t.ticket_id, t.flight_number, t.status, t.price, t.seat_number, "
                  "t.booking_time, f.departure_city, f.arrival_city, f.departure_time "
                  "FROM ticket t "
                  "JOIN flight f ON t.flight_number = f.flight_number "
                  "WHERE t.user_id = ? "
                  "ORDER BY t.booking_time DESC");
    query.addBindValue(userId);
    
    if (query.exec()) {
        while (query.next()) {
            Ticket ticket;
            ticket.ticketId = query.value("ticket_id").toInt();
            ticket.flightNumber = query.value("flight_number").toString();
            ticket.userId = userId;
            ticket.status = query.value("status").toString();
            ticket.price = query.value("price").toDouble();
            ticket.seatNumber = query.value("seat_number").toString();
            ticket.bookingTime = query.value("booking_time").toDateTime();
            
            tickets.append(ticket);
        }
    } else {
        qDebug() << "获取用户票务失败:" << query.lastError().text();
    }
    
    qDebug() << QString("用户%1共有%2张机票").arg(userId).arg(tickets.size());
    return tickets;
}

QVector<Ticket> DatabaseManager::getFlightTickets(const QString& flightNumber) {
    QMutexLocker locker(&dbMutex);
    QVector<Ticket> tickets;
    
    if (flightNumber.isEmpty()) {
        qDebug() << "航班号为空";
        return tickets;
    }
    
    QSqlQuery query(db);
    query.prepare("SELECT ticket_id, flight_number, user_id, status, price, seat_number, booking_time "
                  "FROM ticket WHERE flight_number = ? ORDER BY booking_time DESC");
    query.addBindValue(flightNumber);
    
    if (query.exec()) {
        while (query.next()) {
            Ticket ticket;
            ticket.ticketId = query.value("ticket_id").toInt();
            ticket.flightNumber = query.value("flight_number").toString();
            ticket.userId = query.value("user_id").toInt();
            ticket.status = query.value("status").toString();
            ticket.price = query.value("price").toDouble();
            ticket.seatNumber = query.value("seat_number").toString();
            ticket.bookingTime = query.value("booking_time").toDateTime();
            
            tickets.append(ticket);
        }
    } else {
        qDebug() << "获取航班票务失败:" << query.lastError().text();
    }
    
    qDebug() << QString("航班%1共有%2张机票").arg(flightNumber).arg(tickets.size());
    return tickets;
}

Ticket DatabaseManager::getTicket(int ticketId) {
    QMutexLocker locker(&dbMutex);
    Ticket ticket;
    
    if (ticketId <= 0) {
        qDebug() << "票务ID无效";
        return ticket;
    }
    
    QSqlQuery query(db);
    query.prepare("SELECT ticket_id, flight_number, user_id, status, price, seat_number, booking_time "
                  "FROM ticket WHERE ticket_id = ?");
    query.addBindValue(ticketId);
    
    if (query.exec() && query.next()) {
        ticket.ticketId = query.value("ticket_id").toInt();
        ticket.flightNumber = query.value("flight_number").toString();
        ticket.userId = query.value("user_id").toInt();
        ticket.status = query.value("status").toString();
        ticket.price = query.value("price").toDouble();
        ticket.seatNumber = query.value("seat_number").toString();
        ticket.bookingTime = query.value("booking_time").toDateTime();
    } else {
        qDebug() << "获取票务信息失败:" << ticketId << query.lastError().text();
    }
    
    return ticket;
}

// 预约队列操作实现
bool DatabaseManager::addToReservationQueue(int userId, const QString& flightNumber, int priority) {
    QMutexLocker locker(&dbMutex);
    
    // 检查是否已经在预约队列中
    QSqlQuery query(db);
    query.prepare("SELECT reservation_id FROM reservation_queue WHERE user_id = ? AND flight_number = ?");
    query.addBindValue(userId);
    query.addBindValue(flightNumber);
    
    if (query.exec() && query.next()) {
        qDebug() << QString("用户%1已在航班%2的预约队列中").arg(userId).arg(flightNumber);
        return false;
    }
    
    // 添加到预约队列
    query.prepare("INSERT INTO reservation_queue (flight_number, user_id, priority) VALUES (?, ?, ?)");
    query.addBindValue(flightNumber);
    query.addBindValue(userId);
    query.addBindValue(priority);
    
    if (query.exec()) {
        qDebug() << QString("用户%1成功加入航班%2预约队列，优先级%3")
                    .arg(userId).arg(flightNumber).arg(priority);
        return true;
    } else {
        qDebug() << "加入预约队列失败:" << query.lastError().text();
        return false;
    }
}

bool DatabaseManager::removeFromReservationQueue(int reservationId) {
    QMutexLocker locker(&dbMutex);
    
    QSqlQuery query(db);
    query.prepare("DELETE FROM reservation_queue WHERE reservation_id = ?");
    query.addBindValue(reservationId);
    
    if (query.exec()) {
        qDebug() << QString("从预约队列移除: 预约ID%1").arg(reservationId);
        return true;
    } else {
        qDebug() << "从预约队列移除失败:" << query.lastError().text();
        return false;
    }
}

QVector<Reservation> DatabaseManager::getFlightReservations(const QString& flightNumber) {
    QMutexLocker locker(&dbMutex);
    QVector<Reservation> reservations;
    
    if (flightNumber.isEmpty()) {
        qDebug() << "航班号为空";
        return reservations;
    }
    
    QSqlQuery query(db);
    query.prepare("SELECT r.reservation_id, r.flight_number, r.user_id, r.priority, r.request_time, "
                  "u.username "
                  "FROM reservation_queue r "
                  "LEFT JOIN user u ON r.user_id = u.user_id "
                  "WHERE r.flight_number = ? "
                  "ORDER BY r.priority DESC, r.request_time ASC");
    query.addBindValue(flightNumber);
    
    if (query.exec()) {
        while (query.next()) {
            Reservation reservation;
            reservation.reservationId = query.value("reservation_id").toInt();
            reservation.flightNumber = query.value("flight_number").toString();
            reservation.userId = query.value("user_id").toInt();
            reservation.priority = query.value("priority").toInt();
            reservation.requestTime = query.value("request_time").toDateTime();
            
            reservations.append(reservation);
        }
    } else {
        qDebug() << "获取航班预约失败:" << query.lastError().text();
    }
    
    qDebug() << QString("航班%1共有%2个预约").arg(flightNumber).arg(reservations.size());
    return reservations;
}

QVector<Reservation> DatabaseManager::getUserReservations(int userId) {
    QMutexLocker locker(&dbMutex);
    QVector<Reservation> reservations;
    
    QSqlQuery query(db);
    query.prepare("SELECT r.reservation_id, r.flight_number, r.priority, r.request_time, "
                  "f.departure_city, f.arrival_city, f.departure_time "
                  "FROM reservation_queue r "
                  "JOIN flight f ON r.flight_number = f.flight_number "
                  "WHERE r.user_id = ? "
                  "ORDER BY r.request_time DESC");
    query.addBindValue(userId);
    
    if (query.exec()) {
        while (query.next()) {
            Reservation reservation;
            reservation.reservationId = query.value("reservation_id").toInt();
            reservation.flightNumber = query.value("flight_number").toString();
            reservation.userId = userId;
            reservation.priority = query.value("priority").toInt();
            reservation.requestTime = query.value("request_time").toDateTime();
            
            reservations.append(reservation);
        }
    } else {
        qDebug() << "获取用户预约失败:" << query.lastError().text();
    }
    
    qDebug() << QString("用户%1共有%2个预约").arg(userId).arg(reservations.size());
    return reservations;
}

void DatabaseManager::processReservationQueue(const QString& flightNumber) {
    QMutexLocker locker(&dbMutex);
    
    // 检查是否有可用座位
    QSqlQuery query(db);
    query.prepare("SELECT available_seats FROM flight WHERE flight_number = ?");
    query.addBindValue(flightNumber);
    
    if (!query.exec() || !query.next()) {
        return;
    }
    
    int availableSeats = query.value("available_seats").toInt();
    if (availableSeats <= 0) {
        return; // 没有可用座位
    }
    
    // 获取预约队列中的用户（按优先级和时间排序）
    query.prepare("SELECT reservation_id, user_id, priority FROM reservation_queue "
                  "WHERE flight_number = ? "
                  "ORDER BY priority DESC, request_time ASC "
                  "LIMIT ?");
    query.addBindValue(flightNumber);
    query.addBindValue(availableSeats);
    
    if (query.exec()) {
        QList<QPair<int, int>> reservationsToProcess; // (reservation_id, user_id)
        
        while (query.next()) {
            int reservationId = query.value("reservation_id").toInt();
            int userId = query.value("user_id").toInt();
            reservationsToProcess.append(qMakePair(reservationId, userId));
        }
        
        // 为预约用户自动购票（直接处理，避免递归调用）
        for (const auto& reservation : reservationsToProcess) {
            int reservationId = reservation.first;
            int userId = reservation.second;
            
            // 直接创建票务记录，避免递归调用bookTicket
            // 1. 减少可用座位
            QSqlQuery updateQuery(db);
            updateQuery.prepare("UPDATE flight SET available_seats = available_seats - 1 WHERE flight_number = ? AND available_seats > 0");
            updateQuery.addBindValue(flightNumber);
            
            if (!updateQuery.exec() || updateQuery.numRowsAffected() == 0) {
                break; // 没有更多座位可用
            }
            
            // 2. 获取价格
            QSqlQuery priceQuery(db);
            priceQuery.prepare("SELECT price FROM flight WHERE flight_number = ?");
            priceQuery.addBindValue(flightNumber);
            
            double price = 0.0;
            if (priceQuery.exec() && priceQuery.next()) {
                price = priceQuery.value("price").toDouble();
            }
            
            // 3. 生成座位号
            QString seatNumber = generateSeatNumber(flightNumber);
            
            // 4. 创建票务记录
            QSqlQuery ticketQuery(db);
            ticketQuery.prepare("INSERT INTO ticket (flight_number, user_id, status, price, seat_number, booking_time) "
                              "VALUES (?, ?, ?, ?, ?, datetime('now'))");
            ticketQuery.addBindValue(flightNumber);
            ticketQuery.addBindValue(userId);
            ticketQuery.addBindValue(Constants::TICKET_BOOKED);
            ticketQuery.addBindValue(price);
            ticketQuery.addBindValue(seatNumber);
            
            if (ticketQuery.exec()) {
                // 购票成功，从预约队列中移除
                removeFromReservationQueue(reservationId);
                qDebug() << QString("自动购票成功: 用户%1, 航班%2, 座位%3").arg(userId).arg(flightNumber).arg(seatNumber);
            } else {
                // 回滚座位数量
                QSqlQuery rollbackQuery(db);
                rollbackQuery.prepare("UPDATE flight SET available_seats = available_seats + 1 WHERE flight_number = ?");
                rollbackQuery.addBindValue(flightNumber);
                rollbackQuery.exec();
                qDebug() << "自动购票失败:" << ticketQuery.lastError().text();
                break;
            }
        }
    }
}

// 统计信息实现
int DatabaseManager::getTotalFlights() {
    QMutexLocker locker(&dbMutex);
    
    QSqlQuery query(db);
    if (query.exec("SELECT COUNT(*) FROM flight")) {
        if (query.next()) {
            int count = query.value(0).toInt();
            qDebug() << "航班总数:" << count;
            return count;
        }
    } else {
        qDebug() << "获取航班总数失败:" << query.lastError().text();
    }
    
    return 0;
}

int DatabaseManager::getAvailableFlights() {
    QMutexLocker locker(&dbMutex);
    
    QSqlQuery query(db);
    if (query.exec("SELECT COUNT(*) FROM flight WHERE status = 'Scheduled' AND available_seats > 0")) {
        if (query.next()) {
            int count = query.value(0).toInt();
            qDebug() << "可用航班数:" << count;
            return count;
        }
    } else {
        qDebug() << "获取可用航班数失败:" << query.lastError().text();
    }
    
    return 0;
}

int DatabaseManager::getTotalUsers() {
    QMutexLocker locker(&dbMutex);
    
    QSqlQuery query(db);
    if (query.exec("SELECT COUNT(*) FROM user")) {
        if (query.next()) {
            int count = query.value(0).toInt();
            qDebug() << "用户总数:" << count;
            return count;
        }
    } else {
        qDebug() << "获取用户总数失败:" << query.lastError().text();
    }
    
    return 0;
}

QVector<QString> DatabaseManager::getAllCities() {
    QMutexLocker locker(&dbMutex);
    QVector<QString> cities;
    QSet<QString> citySet; // 使用Set去重
    
    QSqlQuery query(db);
    if (query.exec("SELECT DISTINCT departure_city FROM flight WHERE departure_city IS NOT NULL AND departure_city != ''")) {
        while (query.next()) {
            QString city = query.value(0).toString();
            if (!city.isEmpty() && !citySet.contains(city)) {
                citySet.insert(city);
                cities.append(city);
            }
        }
    }
    
    // 添加目的地城市
    if (query.exec("SELECT DISTINCT arrival_city FROM flight WHERE arrival_city IS NOT NULL AND arrival_city != ''")) {
        while (query.next()) {
            QString city = query.value(0).toString();
            if (!city.isEmpty() && !citySet.contains(city)) {
                citySet.insert(city);
                cities.append(city);
            }
        }
    }
    
    if (cities.isEmpty()) {
        qDebug() << "获取城市列表失败:" << query.lastError().text();
    }
    
    qDebug() << "获取城市数量:" << cities.size();
    return cities;
}

// 工具方法实现
QString DatabaseManager::generateSeatNumber(const QString& flightNumber) {
    QMutexLocker locker(&dbMutex);
    
    // 获取已使用的座位号
    QSqlQuery query(db);
    query.prepare("SELECT seat_number FROM ticket WHERE flight_number = ? AND status != ?");
    query.addBindValue(flightNumber);
    query.addBindValue(Constants::TICKET_CANCELLED);
    
    QSet<QString> usedSeats;
    if (query.exec()) {
        while (query.next()) {
            usedSeats.insert(query.value("seat_number").toString());
        }
    }
    
    // 生成座位号 (A-F列, 1-50排)
    for (int row = 1; row <= 50; ++row) {
        for (char col = 'A'; col <= 'F'; ++col) {
            QString seat = QString("%1%2").arg(row, 2, 10, QChar('0')).arg(col);
            if (!usedSeats.contains(seat)) {
                return seat;
            }
        }
    }
    
    // 如果所有常规座位都满了，生成备用座位
    for (int i = 1; i <= 999; ++i) {
        QString seat = QString("X%1").arg(i, 3, 10, QChar('0'));
        if (!usedSeats.contains(seat)) {
            return seat;
        }
    }
    
    return "XX1"; // 最后的备用座位
}

QVector<int> DatabaseManager::getAffectedUsers(const QString& flightNumber) {
    QMutexLocker locker(&dbMutex);
    QVector<int> userIds;
    
    QSqlQuery query(db);
    // 获取购买该航班机票的用户
    query.prepare("SELECT DISTINCT user_id FROM ticket WHERE flight_number = ? AND status != ?");
    query.addBindValue(flightNumber);
    query.addBindValue(Constants::TICKET_CANCELLED);
    
    if (query.exec()) {
        while (query.next()) {
            userIds.append(query.value("user_id").toInt());
        }
    }
    
    // 获取预约该航班的用户
    query.prepare("SELECT DISTINCT user_id FROM reservation_queue WHERE flight_number = ?");
    query.addBindValue(flightNumber);
    
    if (query.exec()) {
        while (query.next()) {
            int userId = query.value("user_id").toInt();
            if (!userIds.contains(userId)) {
                userIds.append(userId);
            }
        }
    }
    
    return userIds;
}

// 管理员功能实现
QVector<User> DatabaseManager::getAllUsers() {
    QMutexLocker locker(&dbMutex);
    QVector<User> users;
    
    QSqlQuery query(db);
    query.prepare("SELECT user_id, username, email, phone, user_type FROM user ORDER BY user_id");
    
    if (query.exec()) {
        while (query.next()) {
            User user;
            user.userId = query.value("user_id").toInt();
            user.username = query.value("username").toString();
            user.email = query.value("email").toString();
            user.phone = query.value("phone").toString();
            user.userType = query.value("user_type").toString();
            
            users.append(user);
        }
    } else {
        qDebug() << "获取所有用户失败:" << query.lastError().text();
    }
    
    return users;
}

bool DatabaseManager::resetUserPassword(int userId, const QString& newPassword) {
    QMutexLocker locker(&dbMutex);
    
    QSqlQuery query(db);
    query.prepare("UPDATE user SET password = ? WHERE user_id = ?");
    query.addBindValue(newPassword); // 直接使用明文密码
    query.addBindValue(userId);
    
    if (query.exec()) {
        qDebug() << QString("重置用户%1密码成功").arg(userId);
        return true;
    } else {
        qDebug() << "重置密码失败:" << query.lastError().text();
        return false;
    }
}

QVector<Ticket> DatabaseManager::getAllTickets() {
    QMutexLocker locker(&dbMutex);
    QVector<Ticket> tickets;
    
    QSqlQuery query(db);
    query.prepare("SELECT ticket_id, flight_number, user_id, status, price, seat_number, booking_time "
                  "FROM ticket ORDER BY booking_time DESC");
    
    if (query.exec()) {
        while (query.next()) {
            Ticket ticket;
            ticket.ticketId = query.value("ticket_id").toInt();
            ticket.flightNumber = query.value("flight_number").toString();
            ticket.userId = query.value("user_id").toInt();
            ticket.status = query.value("status").toString();
            ticket.price = query.value("price").toDouble();
            ticket.seatNumber = query.value("seat_number").toString();
            ticket.bookingTime = query.value("booking_time").toDateTime();
            
            tickets.append(ticket);
        }
    } else {
        qDebug() << "获取所有票务失败:" << query.lastError().text();
    }
    
    return tickets;
}

double DatabaseManager::getTotalRevenue(const QDate& fromDate, const QDate& toDate) {
    QMutexLocker locker(&dbMutex);
    
    QSqlQuery query(db);
    QString sql = "SELECT SUM(price) FROM ticket WHERE status = ?";
    
    if (fromDate.isValid() && toDate.isValid()) {
        sql += " AND DATE(booking_time) BETWEEN ? AND ?";
    }
    
    query.prepare(sql);
    query.addBindValue(Constants::TICKET_BOOKED);
    
    if (fromDate.isValid() && toDate.isValid()) {
        query.addBindValue(fromDate.toString("yyyy-MM-dd"));
        query.addBindValue(toDate.toString("yyyy-MM-dd"));
    }
    
    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }
    
    return 0.0;
}

int DatabaseManager::getFlightCount(const QDate& fromDate, const QDate& toDate) {
    QMutexLocker locker(&dbMutex);
    
    QSqlQuery query(db);
    QString sql = "SELECT COUNT(*) FROM flight";
    
    if (fromDate.isValid() && toDate.isValid()) {
        sql += " WHERE DATE(departure_time) BETWEEN ? AND ?";
    }
    
    query.prepare(sql);
    
    if (fromDate.isValid() && toDate.isValid()) {
        query.addBindValue(fromDate.toString("yyyy-MM-dd"));
        query.addBindValue(toDate.toString("yyyy-MM-dd"));
    }
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
}

int DatabaseManager::getUserCount(const QDate& fromDate, const QDate& toDate) {
    Q_UNUSED(fromDate)
    Q_UNUSED(toDate)
    // 用户数量通常不按日期统计，返回总用户数
    return getTotalUsers();
}

int DatabaseManager::getTicketCount(const QDate& fromDate, const QDate& toDate) {
    QMutexLocker locker(&dbMutex);
    
    QSqlQuery query(db);
    QString sql = "SELECT COUNT(*) FROM ticket WHERE status = ?";
    
    if (fromDate.isValid() && toDate.isValid()) {
        sql += " AND DATE(booking_time) BETWEEN ? AND ?";
    }
    
    query.prepare(sql);
    query.addBindValue(Constants::TICKET_BOOKED);
    
    if (fromDate.isValid() && toDate.isValid()) {
        query.addBindValue(fromDate.toString("yyyy-MM-dd"));
        query.addBindValue(toDate.toString("yyyy-MM-dd"));
    }
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
} 