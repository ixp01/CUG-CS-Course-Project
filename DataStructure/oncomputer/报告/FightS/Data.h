#ifndef DATA_H
#define DATA_H

#include <string>
#include <vector>
#include <QDateTime>
#include <queue>
#include <QList>
#include <QStringList>
#include <ostream>
#include <iostream>
#include <QTextStream>
#include <QMutex>
#include <QFile>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QString>
#include <QHash>
#include <QLockFile>

using namespace std;

// 前置声明
class RouteEdge;
class CityNode;
class Flight;
class Ticket;

// 操作类型枚举
enum class OperationType {
    FlightAdd,
    FlightUpdate,
    FlightDelete,
    TicketPurchase,
    TicketCancel
};

// 操作记录结构
struct Operation {
    OperationType type;
    QString data;
    QDateTime timestamp;
    QString clientId;
    
    Operation() = default;
    Operation(OperationType t, const QString& d, const QString& cId) 
        : type(t), data(d), timestamp(QDateTime::currentDateTime()), clientId(cId) {}
};

// 改进的文件锁管理器类
class FileLockManager {
public:
    static FileLockManager* getInstance();
    
    // 获取文件锁（阻塞式，带超时）
    bool acquireLock(const QString& filename, int timeoutMs = 5000);
    
    // 释放文件锁
    void releaseLock(const QString& filename);
    
    // 检查文件是否被锁定
    bool isLocked(const QString& filename);
    
    // 记录操作到同步文件
    void recordOperation(const Operation& op);
    
    // 获取最新操作
    QList<Operation> getRecentOperations(const QDateTime& since);
    
    // 清理过期的锁文件
    void cleanupStaleLocks();
    
private:
    FileLockManager();
    ~FileLockManager();
    
    static FileLockManager* instance;
    QMutex mutex;
    QHash<QString, QLockFile*> lockFiles;  // 使用QLockFile
    QString syncLogFile;  // 同步日志文件
    QString clientId;     // 当前客户端ID
    
    QString getLockFileName(const QString& filename);
    QString generateClientId();
};

enum class FlightStatus {
    Normal,    // 正常
    Delayed,   // 延误
    Cancelled  // 取消
};

enum class SortCriteria { Price, Duration };

// 航班信息类
class Flight {
public:
    Flight();  // 默认构造函数
    Flight(const string& id, const string& air, const QDateTime& dep, 
           const QDateTime& arr, const string& from, const string& to,
           int seats, double p);
    Flight(const Flight& other);  // 拷贝构造函数

    string flightID;         // 航班号
    string airline;          // 航空公司
    QDateTime departureTime;      // 起飞时间
    QDateTime arrivalTime;        // 降落时间
    vector<string> viaCities;  // 经停地点
    string fromCity;         // 出发城市
    string toCity;           // 到达城市
    int totalSeats;               // 总座位数
    int availableSeats;           // 可售票数
    double price;                 // 票价
    FlightStatus status;          // 航班状态

    // 序列化到文本文件
    friend QTextStream& operator<<(QTextStream& os, const Flight& flight) {
        os << QString::fromStdString(flight.flightID) << '\n'
           << QString::fromStdString(flight.airline) << '\n'
           << flight.departureTime.toString("yyyy-MM-dd hh:mm") << '\n'
           << flight.arrivalTime.toString("yyyy-MM-dd hh:mm") << '\n'
           << QString::fromStdString(flight.fromCity) << '\n'
           << QString::fromStdString(flight.toCity) << '\n'
           << flight.totalSeats << '\n'
           << flight.availableSeats << '\n'
           << flight.price << '\n'
           << static_cast<int>(flight.status) << '\n';
        
        // 写入经停城市
        os << flight.viaCities.size() << '\n';
        for (const auto& city : flight.viaCities) {
            os << QString::fromStdString(city) << '\n';
        }
        return os;
    }

    // 从文本文件反序列化
    friend QTextStream& operator>>(QTextStream& is, Flight& flight) {
        QString line;
        
        // 读取基本信息
        flight.flightID = is.readLine().toStdString();
        flight.airline = is.readLine().toStdString();
        
        flight.departureTime = QDateTime::fromString(is.readLine(), "yyyy-MM-dd hh:mm");
        flight.arrivalTime = QDateTime::fromString(is.readLine(), "yyyy-MM-dd hh:mm");
        
        flight.fromCity = is.readLine().toStdString();
        flight.toCity = is.readLine().toStdString();
        
        flight.totalSeats = is.readLine().toInt();
        flight.availableSeats = is.readLine().toInt();
        flight.price = is.readLine().toDouble();
        flight.status = static_cast<FlightStatus>(is.readLine().toInt());
        
        // 读取经停城市
        int viaCount = is.readLine().toInt();
        
        flight.viaCities.clear();
        for (int i = 0; i < viaCount; ++i) {
            flight.viaCities.push_back(is.readLine().toStdString());
        }
        
        return is;
    }

    // 转换为QString的辅助函数
    QString getFlightID() const { return QString::fromStdString(flightID); }
    QString getAirline() const { return QString::fromStdString(airline); }
    QString getFromCity() const { return QString::fromStdString(fromCity); }
    QString getToCity() const { return QString::fromStdString(toCity); }
    QStringList getViaCities() const {
        QStringList result;
        for (const auto& city : viaCities) {
            result << QString::fromStdString(city);
        }
        return result;
    }
};

// 城市节点类（用于图结构）
class CityNode {
public:
    CityNode();  // 默认构造函数
    ~CityNode();  // 析构函数

    string cityName;         // 城市名称
    vector<RouteEdge*> edges;  // 邻接边列表

    QString getCityName() const { return QString::fromStdString(cityName); }
};

// 航线边类（图的边）
class RouteEdge {
public:
    RouteEdge();  // 默认构造函数
    ~RouteEdge();  // 析构函数

    CityNode* fromCity;       // 出发城市
    CityNode* toCity;         // 到达城市
    Flight* flight;           // 关联的航班
    int price;                // 价格权重
    int duration;             // 时间权重（分钟）
};

// 票务信息类
class Ticket {
public:
    string ticketID;         // 票号
    string passengerName;    // 乘客姓名
    Flight* flight;               // 关联航班
    bool isReserved;              // 是否预约抢票
    QDateTime purchaseTime;       // 购票时间

    Ticket();  // 默认构造函数
    Ticket(const string& pid, const string& pname, Flight* f);  // 带参数的构造函数
    ~Ticket();  // 析构函数

    QString getTicketID() const { return QString::fromStdString(ticketID); }
    QString getPassengerName() const { return QString::fromStdString(passengerName); }
    
    // 生成唯一票号
    static string generateTicketID() {
        static int counter = 0;
        return "T" + std::to_string(QDateTime::currentDateTime().toSecsSinceEpoch()) 
               + std::to_string(++counter);
    }
};

class Reservation {
public:
    QDateTime timestamp;
    string passenger;
    Flight* flight = nullptr;

    bool operator<(const Reservation& other) const {
        if (timestamp != other.timestamp)
            return timestamp > other.timestamp;
        else
            return flight->status < other.flight->status;
    }

    QString getPassenger() const { return QString::fromStdString(passenger); }
};

#endif // DATA_H

