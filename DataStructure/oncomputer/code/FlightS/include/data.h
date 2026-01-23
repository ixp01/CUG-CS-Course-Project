#ifndef DATA_H
#define DATA_H

#include "common.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVector>
#include <QString>
#include <QDateTime>
#include <QStringList>
#include <QMutex>
#include <QDebug>

// 航班信息结构体
struct Flight {
    QString flightNumber;
    QString airline;
    QString departureCity;
    QString arrivalCity;
    QDateTime departureTime;
    QDateTime arrivalTime;
    QStringList stopovers;
    int totalSeats;
    int availableSeats;
    QString status;
    double price;  // 添加价格字段
    
    Flight() : totalSeats(0), availableSeats(0), price(0.0) {}
    
    bool isValid() const {
        return !flightNumber.isEmpty() && 
               !departureCity.isEmpty() && 
               !arrivalCity.isEmpty() &&
               departureTime.isValid() &&
               arrivalTime.isValid() &&
               totalSeats > 0;
    }
};

// 用户信息结构体
struct User {
    int userId;
    QString username;
    QString password;
    QString email;
    QString phone;
    QString userType;
    
    User() : userId(-1) {}
    
    bool isValid() const {
        return userId > 0 && !username.isEmpty() && !password.isEmpty();
    }
};

// 管理员信息结构体
struct Manager {
    int managerId;
    QString username;
    QString password;
    
    Manager() : managerId(-1) {}
    
    bool isValid() const {
        return managerId > 0 && !username.isEmpty() && !password.isEmpty();
    }
};

// 票务信息结构体
struct Ticket {
    int ticketId;
    QString flightNumber;
    int userId;
    QString status;
    double price;
    QString seatNumber;
    QDateTime bookingTime;
    
    Ticket() : ticketId(-1), userId(-1), price(0.0) {}
    
    bool isValid() const {
        return ticketId > 0 && !flightNumber.isEmpty() && userId > 0;
    }
};

// 预约信息结构体
struct Reservation {
    int reservationId;
    QString flightNumber;
    int userId;
    QDateTime requestTime;
    int priority;
    
    Reservation() : reservationId(-1), userId(-1), priority(0) {}
    
    bool isValid() const {
        return reservationId > 0 && !flightNumber.isEmpty() && userId > 0;
    }
};

// 转机方案结构体
struct TransferPlan {
    QVector<Flight> flights;
    double totalPrice;
    QDateTime departureTime;
    QDateTime arrivalTime;
    int transferCount;
    
    TransferPlan() : totalPrice(0.0), transferCount(0) {}
    
    QDateTime totalDuration() const {
        if (flights.isEmpty()) return QDateTime();
        return flights.last().arrivalTime;
    }
};

// 数据库管理类
class DatabaseManager {
public:
    // 单例模式
    static DatabaseManager& instance();
    
    // 数据库连接
    bool connectToDatabase(const QString& dbPath = Constants::DATABASE_PATH);
    void closeDatabase();
    
    // 用户相关操作
    bool registerUser(const QString& username, const QString& password, 
                     const QString& email, const QString& phone, 
                     const QString& userType = Constants::USER_TYPE_NORMAL);
    int authenticateUser(const QString& username, const QString& password);
    User getUserInfo(int userId);
    bool updateUserInfo(const User& user);
    
    // 管理员相关操作
    int authenticateManager(const QString& username, const QString& password);
    Manager getManagerInfo(int managerId);
    
    // 航班相关操作
    QVector<Flight> getAllFlights();
    QVector<Flight> searchFlights(const QString& from = QString(), 
                                 const QString& to = QString(),
                                 const QDate& date = QDate());
    Flight getFlight(const QString& flightNumber);
    bool addFlight(const Flight& flight);
    bool updateFlight(const Flight& flight);
    bool deleteFlight(const QString& flightNumber);
    bool updateFlightStatus(const QString& flightNumber, const QString& status);
    bool updateFlightTime(const QString& flightNumber, 
                         const QDateTime& newDep, const QDateTime& newArr);
    
    // 票务相关操作
    TicketResult bookTicket(int userId, const QString& flightNumber);
    bool refundTicket(int ticketId);
    QVector<Ticket> getUserTickets(int userId);
    QVector<Ticket> getFlightTickets(const QString& flightNumber);
    Ticket getTicket(int ticketId);
    
    // 预约队列操作
    bool addToReservationQueue(int userId, const QString& flightNumber, int priority = Constants::PRIORITY_NORMAL);
    bool removeFromReservationQueue(int reservationId);
    QVector<Reservation> getFlightReservations(const QString& flightNumber);
    QVector<Reservation> getUserReservations(int userId);
    void processReservationQueue(const QString& flightNumber);
    
    // 统计信息
    int getTotalFlights();
    int getAvailableFlights();
    int getTotalUsers();
    QVector<QString> getAllCities();
    
    // 管理员功能
    QVector<User> getAllUsers();
    bool resetUserPassword(int userId, const QString& newPassword);
    QVector<Ticket> getAllTickets();
    double getTotalRevenue(const QDate& fromDate = QDate(), const QDate& toDate = QDate());
    int getFlightCount(const QDate& fromDate = QDate(), const QDate& toDate = QDate());
    int getUserCount(const QDate& fromDate = QDate(), const QDate& toDate = QDate());
    int getTicketCount(const QDate& fromDate = QDate(), const QDate& toDate = QDate());
    
    // 工具方法
    QString generateSeatNumber(const QString& flightNumber);
    QVector<int> getAffectedUsers(const QString& flightNumber);

private:
    DatabaseManager() = default;
    ~DatabaseManager();
    
    // 禁用拷贝构造和赋值
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    
    // 数据库连接
    QSqlDatabase db;
    static QMutex dbMutex;
    
    // 私有方法
    bool executeQuery(QSqlQuery& query, const QString& errorMsg = QString());
    QString hashPassword(const QString& password);
    bool validateEmail(const QString& email);
    bool validatePhone(const QString& phone);
};

#endif // DATA_H 