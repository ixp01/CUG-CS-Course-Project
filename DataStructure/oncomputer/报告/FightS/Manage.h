#ifndef MANAGE_H
#define MANAGE_H

#include "Data.h"
#include <QObject>
#include <QList>
#include <QHash>
#include <QMutex>
#include <QQueue>
#include <QDateTime>
#include <QTimer>
#include <QFileSystemWatcher>


// ==================== 航班管理器 ====================
class FlightManager : public QObject {
    Q_OBJECT
public:
    explicit FlightManager(QObject* parent = nullptr);
    ~FlightManager();  // 添加析构函数声明
    
    // 文件操作
    bool loadFromFile(const QString& filename);
    bool saveToFile(const QString& filename);
    
    // 带锁的文件操作
    bool loadFromFileWithLock(const QString& filename);
    bool saveToFileWithLock(const QString& filename);
    
    // 设置数据文件路径并启动监控
    void setDataFilePath(const QString& filePath);
    void startFileMonitoring();
    void stopFileMonitoring();
    
    // 航班操作
    void addFlight(const Flight& flight);
    bool deleteFlight(const QString& flightID);
    bool updateFlight(const QString& flightID, const Flight& newData);
    Flight* findFlight(const QString& flightID); // O(1)快速查找
    
    // 状态管理
    void setFlightStatus(const QString& flightID, FlightStatus status);
    
    // 获取所有航班（用于界面显示）
    const QList<Flight*>& getAllFlights() const { return flights; }

    // 搜索功能
    QList<Flight*> searchByCity(const QString& fromCity, const QString& toCity);
    QList<Flight*> searchByTimeRange(const QDateTime& startTime, const QDateTime& endTime);
    QList<Flight*> searchByPriceRange(double minPrice, double maxPrice);
    
    // 排序功能
    void sortFlights(QList<Flight*>& flights, SortCriteria criteria);
    
    // 组合查询
    QList<Flight*> advancedSearch(const QString& fromCity, const QString& toCity,
                                const QDateTime& startTime, const QDateTime& endTime,
                                double minPrice, double maxPrice,
                                SortCriteria sortBy);

    void clearAllData();  // 清空所有数据

public slots:
    void onFileChanged(const QString& path);  // 文件变化处理

signals:
    void flightAdded(Flight* newFlight);
    void flightRemoved(QString flightID);
    void flightStatusChanged(QString flightID, FlightStatus newStatus);
    void dataFileChanged();  // 数据文件变化信号
    void dataModified();     // 数据修改信号（新增）

private:
    QList<Flight*> flights;       // 线性表存储
    QHash<QString, Flight*> flightIDMap; // 航班号哈希索引
    QHash<QString, QList<Flight*>> fromCityIndex;  // 出发城市索引
    QHash<QString, QList<Flight*>> toCityIndex;    // 到达城市索引
    QList<Flight*> timeSortedFlights;             // 按时间排序的航班列表
    QList<Flight*> priceSortedFlights;            // 按价格排序的航班列表
    
    // 文件监控相关
    QString dataFilePath;
    QFileSystemWatcher* fileWatcher;
    QTimer* reloadTimer = nullptr;  // 添加重载定时器
    QDateTime lastSaveTime;  // 记录上次保存时间，避免循环监控
    QString lastOperationId; // 新增：记录最近一次操作ID
    
    void updateIndices(Flight* flight);           // 更新索引
    void removeFromIndices(Flight* flight);       // 从索引中移除
    bool isValidTimeRange(const QDateTime& start, const QDateTime& end);
    
    void delayedReload();  // 延迟重载数据
};

// ==================== 票务管理器 ====================
class TicketManager : public QObject {
    Q_OBJECT
public:
    explicit TicketManager(FlightManager* flightMgr, QObject* parent = nullptr);
    ~TicketManager();
    
    // 票务操作
    Ticket* purchaseTicket(const QString& flightID, const QString& passenger);
    
    // 同步购票操作（带文件锁）
    Ticket* purchaseTicketWithLock(const QString& flightID, const QString& passenger);
    
    bool refundTicket(const QString& ticketID);
    
    // 预约队列管理
    void addReservation(const QString& flightID, const QString& passenger);
    void processReservations(); // 定时检查余票分配
    void processReservation();
    void clearReservations();
    
    // 票务查询功能
    Ticket* findTicketByID(const QString& ticketID);
    QList<Ticket*> findTicketsByPassenger(const QString& passengerName);
    QList<Ticket*> findTicketsByFlight(const QString& flightID);
    
    // 获取所有票务信息
    QList<Ticket*> getAllTickets(); // 返回所有票务列表
    
    // 获取特定航班的所有乘客
    QList<Ticket*> getTicketsByFlightID(const QString& flightID);
    
    // 内部无锁版本（已在外部获得锁的情况下调用）
    QList<Ticket*> getTicketsByFlightIDInternal(const QString& flightID);

    // 添加票务记录（用于加载数据）
    void addTicket(Ticket* ticket);
    void clearAllData();
    
    // 文件读写功能
    bool loadFromFile(const QString& filename);
    bool saveToFile(const QString& filename);
    bool saveToFileWithLock(const QString& filename); // 带锁保存票务数据
    void delayedReload();  // 添加延迟重载函数声明
    
    // 文件监控相关
    void setDataFilePath(const QString& filePath);
    void startFileMonitoring();
    void stopFileMonitoring();

signals:
    void ticketPurchased(Ticket* ticket);
    void ticketRefunded(QString ticketID);
    void reservationFulfilled(QString passenger, QString flightID);
    void ticketFound(Ticket* ticket);
    void ticketsFound(const QList<Ticket*>& tickets);

public slots:
    void onFileChanged(const QString& path);  // 添加文件变化处理槽函数

private:
    struct Reservation {
        QString flightID;
        QString passenger;
        QDateTime timestamp;
    };
    
    FlightManager* flightManager;
    QHash<QString, Ticket*> tickets;  // 使用哈希表存储票务信息
    QQueue<Reservation> reservations;  // 预约队列
    
    // 文件监控相关成员变量
    QString dataFilePath;
    QFileSystemWatcher* fileWatcher;
    QTimer* reloadTimer = nullptr;
    
    bool isValidPurchase(Flight* flight, const QString& passenger);
    Ticket* createTicket(Flight* flight, const QString& passenger);
    
    // 内部购票逻辑（不加锁）
    Ticket* purchaseTicketInternal(const QString& flightID, const QString& passenger);
    
    QDateTime lastSaveTime;
    QString lastOperationId;
};




// ==================== 航线图管理器 ====================
class RouteGraphManager : public QObject {
    Q_OBJECT
public:
    explicit RouteGraphManager(FlightManager* flightMgr, QObject *parent = nullptr);
    
    // 图操作
    void rebuildGraph();
    QList<Flight*> findShortestPath(const QString& fromCity, const QString& toCity, SortCriteria criteria);

private:
    FlightManager* flightManager;
    QHash<QString, CityNode*> cityMap;  // 城市名到节点的映射
    
    bool isValidCities(const QString& fromCity, const QString& toCity);
    void initializeDijkstra(const QString& fromCity, QHash<CityNode*, int>& dist);
    CityNode* findMinDistanceNode(const QList<CityNode*>& nodes, const QHash<CityNode*, int>& dist);
    void updateNeighbors(CityNode* current, QHash<CityNode*, int>& dist,
                        QHash<CityNode*, RouteEdge*>& prev, SortCriteria criteria);
    QList<Flight*> reconstructPath(const QString& toCity, const QHash<CityNode*, RouteEdge*>& prev);
};




// ==================== 通知管理器 ====================
class NotificationManager : public QObject {
    Q_OBJECT
public:
    explicit NotificationManager(FlightManager* flightMgr,
                               RouteGraphManager* routeMgr,
                               QObject *parent = nullptr);
    
    void startMonitoring(); // 启动状态监控
    void sendNotification(const QString& message);

public slots:
    void onFlightStatusChanged(QString flightID, FlightStatus status);

signals:
    void notificationGenerated(QString message);

private:
    FlightManager* flightManager;
    RouteGraphManager* routeGraphManager;
    QTimer* monitorTimer; // 定时检查状态
};

#endif // MANAGE_H
