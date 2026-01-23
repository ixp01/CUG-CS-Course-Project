#include "Manage.h"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <algorithm>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QFileInfo>
#include <QMutexLocker>

// ==================== FlightManager实现 ====================
FlightManager::FlightManager(QObject *parent)
    : QObject(parent), fileWatcher(nullptr), reloadTimer(nullptr)
{
    // 初始化文件监控器
    fileWatcher = new QFileSystemWatcher(this);
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &FlightManager::onFileChanged);
    
    // 初始化延迟重载定时器
    reloadTimer = new QTimer(this);
    reloadTimer->setSingleShot(true);
    reloadTimer->setInterval(500); // 500ms延迟
    connect(reloadTimer, &QTimer::timeout, this, &FlightManager::delayedReload);
}

FlightManager::~FlightManager()
{
    stopFileMonitoring();
    clearAllData();
}

void FlightManager::setDataFilePath(const QString& filePath)
{
    dataFilePath = filePath;
}

void FlightManager::startFileMonitoring()
{
    if (!dataFilePath.isEmpty() && QFile::exists(dataFilePath)) {
        if (!fileWatcher->files().contains(dataFilePath)) {
            fileWatcher->addPath(dataFilePath);
        }
    }
}

void FlightManager::stopFileMonitoring()
{
    if (fileWatcher && !fileWatcher->files().isEmpty()) {
        fileWatcher->removePaths(fileWatcher->files());
    }
}

void FlightManager::onFileChanged(const QString& path)
{
    if (path == dataFilePath) {
        // 使用定时器延迟重载，避免频繁刷新
        reloadTimer->start();
    }
}

void FlightManager::delayedReload()
{
    if (loadFromFileWithLock(dataFilePath)) {
        emit dataFileChanged();
    }
}

bool FlightManager::loadFromFileWithLock(const QString& filename)
{
    FileLockManager* lockManager = FileLockManager::getInstance();
    
    if (!lockManager->acquireLock(filename, 3000)) {
        return false; // 获取锁失败
    }
    
    bool result = loadFromFile(filename);
    
    lockManager->releaseLock(filename);
    return result;
}

bool FlightManager::saveToFileWithLock(const QString& filename)
{
    FileLockManager* lockManager = FileLockManager::getInstance();
    
    if (!lockManager->acquireLock(filename, 3000)) {
        return false; // 获取锁失败
    }
    
    bool result = saveToFile(filename);
    
    lockManager->releaseLock(filename);
    return result;
}

bool FlightManager::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    clearAllData();
    QTextStream in(&file);
    while (!in.atEnd()) {
        Flight flight;
        
        // 读取基本信息（10行）
        if (in.atEnd()) break;
        flight.flightID = in.readLine().trimmed().toStdString();
        if (in.atEnd()) break;
        flight.airline = in.readLine().trimmed().toStdString();
        if (in.atEnd()) break;
        flight.departureTime = QDateTime::fromString(in.readLine().trimmed(), "yyyy-MM-dd hh:mm");
        if (in.atEnd()) break;
        flight.arrivalTime = QDateTime::fromString(in.readLine().trimmed(), "yyyy-MM-dd hh:mm");
        if (in.atEnd()) break;
        flight.fromCity = in.readLine().trimmed().toStdString();
        if (in.atEnd()) break;
        flight.toCity = in.readLine().trimmed().toStdString();
        if (in.atEnd()) break;
        flight.totalSeats = in.readLine().trimmed().toInt();
        if (in.atEnd()) break;
        flight.availableSeats = in.readLine().trimmed().toInt();
        if (in.atEnd()) break;
        flight.price = in.readLine().trimmed().toDouble();
        if (in.atEnd()) break;
        flight.status = static_cast<FlightStatus>(in.readLine().trimmed().toInt());
        
        // 读取经停城市数量
        if (in.atEnd()) break;
        int viaCount = in.readLine().trimmed().toInt();
        
        // 读取经停城市列表
        flight.viaCities.clear();
        for (int i = 0; i < viaCount && !in.atEnd(); ++i) {
            flight.viaCities.push_back(in.readLine().trimmed().toStdString());
        }
        
        // 验证必要字段
        if (flight.flightID.empty() || flight.airline.empty() || 
            !flight.departureTime.isValid() || !flight.arrivalTime.isValid()) {
            continue; // 跳过无效数据
        }
        
        addFlight(flight);
    }
    return true;
}

bool FlightManager::saveToFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件进行写入：" << filename;
        return false;
    }

    QTextStream out(&file);
    
    try {
        qDebug() << "开始保存航班数据，共" << flights.size() << "个航班";
        
        for (const Flight* flight : flights) {
            if (!flight) {
                qDebug() << "跳过空航班指针";
                continue; // 跳过空指针
            }
            
            // 安全地写入每个字段
            out << QString::fromStdString(flight->flightID) << '\n';
            out << QString::fromStdString(flight->airline) << '\n';
            out << flight->departureTime.toString("yyyy-MM-dd hh:mm") << '\n';
            out << flight->arrivalTime.toString("yyyy-MM-dd hh:mm") << '\n';
            out << QString::fromStdString(flight->fromCity) << '\n';
            out << QString::fromStdString(flight->toCity) << '\n';
            out << flight->totalSeats << '\n';
            out << flight->availableSeats << '\n';
            out << flight->price << '\n';
            out << static_cast<int>(flight->status) << '\n';
            
            // 写入经停城市
            out << flight->viaCities.size() << '\n';
            for (const auto& city : flight->viaCities) {
                out << QString::fromStdString(city) << '\n';
            }
            
            out.flush(); // 确保数据写入
        }
        
        file.flush();
        qDebug() << "航班数据保存完成";
        return true;
        
    } catch (const std::exception& e) {
        qDebug() << "保存时发生异常：" << e.what();
        return false;
    } catch (...) {
        qDebug() << "保存时发生未知异常";
        return false;
    }
}

void FlightManager::addFlight(const Flight& flight)
{
    Flight* newFlight = new Flight(flight);
    flights.append(newFlight);
    flightIDMap[QString::fromStdString(newFlight->flightID)] = newFlight;
    updateIndices(newFlight);
    emit flightAdded(newFlight);
}

bool FlightManager::deleteFlight(const QString& flightID)
{
    Flight* flight = flightIDMap.value(flightID);
    if (!flight) return false;

    removeFromIndices(flight);
    flightIDMap.remove(flightID);
    flights.removeOne(flight);
    delete flight;
    emit flightRemoved(flightID);
    return true;
}

bool FlightManager::updateFlight(const QString& flightID, const Flight& newData)
{
    Flight* flight = flightIDMap.value(flightID);
    if (!flight) return false;

    removeFromIndices(flight);
    *flight = newData;
    updateIndices(flight);
    return true;
}

Flight* FlightManager::findFlight(const QString& flightID)
{
    return flightIDMap.value(flightID);
}

void FlightManager::setFlightStatus(const QString& flightID, FlightStatus status)
{
    Flight* flight = flightIDMap.value(flightID);
    if (flight && flight->status != status) {
        flight->status = status;
        emit flightStatusChanged(flightID, status);
    }
}

QList<Flight*> FlightManager::searchByCity(const QString& fromCity, const QString& toCity)
{
    QList<Flight*> result;
    if (fromCity.isEmpty() && toCity.isEmpty()) return result;

    if (!fromCity.isEmpty() && !toCity.isEmpty()) {
        // 同时匹配出发地和目的地
        QList<Flight*> fromFlights = fromCityIndex.value(fromCity);
        for (Flight* flight : fromFlights) {
            if (QString::fromStdString(flight->toCity) == toCity) {
                result.append(flight);
            }
        }
    } else if (!fromCity.isEmpty()) {
        result = fromCityIndex.value(fromCity);
    } else {
        result = toCityIndex.value(toCity);
    }
    return result;
}

QList<Flight*> FlightManager::searchByTimeRange(const QDateTime& startTime, const QDateTime& endTime)
{
    if (!isValidTimeRange(startTime, endTime)) return QList<Flight*>();

    QList<Flight*> result;
    for (Flight* flight : flights) {
        if (flight->departureTime >= startTime && flight->departureTime <= endTime) {
            result.append(flight);
        }
    }
    return result;
}

QList<Flight*> FlightManager::searchByPriceRange(double minPrice, double maxPrice)
{
    QList<Flight*> result;
    for (Flight* flight : flights) {
        if (flight->price >= minPrice && flight->price <= maxPrice) {
            result.append(flight);
        }
    }
    return result;
}

void FlightManager::sortFlights(QList<Flight*>& flights, SortCriteria criteria)
{
    switch (criteria) {
        case SortCriteria::Price:
            std::sort(flights.begin(), flights.end(),
                     [](Flight* a, Flight* b) { return a->price < b->price; });
            break;
        case SortCriteria::Duration:
            std::sort(flights.begin(), flights.end(),
                     [](Flight* a, Flight* b) {
                         return a->departureTime.secsTo(a->arrivalTime) <
                                b->departureTime.secsTo(b->arrivalTime);
                     });
            break;
    }
}

QList<Flight*> FlightManager::advancedSearch(const QString& fromCity, const QString& toCity,
                                           const QDateTime& startTime, const QDateTime& endTime,
                                           double minPrice, double maxPrice,
                                           SortCriteria sortBy)
{
    QList<Flight*> result = searchByCity(fromCity, toCity);
    if (result.isEmpty()) return result;

    // 时间过滤
    if (startTime.isValid() && endTime.isValid()) {
        result = searchByTimeRange(startTime, endTime);
    }

    // 价格过滤
    if (minPrice >= 0 && maxPrice > minPrice) {
        result = searchByPriceRange(minPrice, maxPrice);
    }

    // 排序
    if (!result.isEmpty()) {
        sortFlights(result, sortBy);
    }

    return result;
}

void FlightManager::clearAllData()
{
    for (Flight* flight : flights) {
        delete flight;
    }
    flights.clear();
    flightIDMap.clear();
    fromCityIndex.clear();
    toCityIndex.clear();
    timeSortedFlights.clear();
    priceSortedFlights.clear();
}

void FlightManager::updateIndices(Flight* flight)
{
    fromCityIndex[QString::fromStdString(flight->fromCity)].append(flight);
    toCityIndex[QString::fromStdString(flight->toCity)].append(flight);
    timeSortedFlights.append(flight);
    priceSortedFlights.append(flight);
    sortFlights(timeSortedFlights, SortCriteria::Duration);
    sortFlights(priceSortedFlights, SortCriteria::Price);
}

void FlightManager::removeFromIndices(Flight* flight)
{
    fromCityIndex[QString::fromStdString(flight->fromCity)].removeOne(flight);
    toCityIndex[QString::fromStdString(flight->toCity)].removeOne(flight);
    timeSortedFlights.removeOne(flight);
    priceSortedFlights.removeOne(flight);
}

bool FlightManager::isValidTimeRange(const QDateTime& start, const QDateTime& end)
{
    return start.isValid() && end.isValid() && start <= end;
}

// ==================== TicketManager实现 ====================
TicketManager::TicketManager(FlightManager* flightMgr, QObject *parent)
    : QObject(parent)
    , flightManager(flightMgr)
    , fileWatcher(nullptr)
    , reloadTimer(nullptr)
{
    // 初始化文件监控器
    fileWatcher = new QFileSystemWatcher(this);
    connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &TicketManager::onFileChanged);
    
    // 初始化延迟重载定时器
    reloadTimer = new QTimer(this);
    reloadTimer->setSingleShot(true);
    reloadTimer->setInterval(500); // 500ms延迟
    connect(reloadTimer, &QTimer::timeout, this, &TicketManager::delayedReload);
}

TicketManager::~TicketManager()
{
    clearAllData();
}

Ticket* TicketManager::purchaseTicket(const QString& flightID, const QString& passenger)
{
    return purchaseTicketInternal(flightID, passenger);
}

Ticket* TicketManager::purchaseTicketWithLock(const QString& flightID, const QString& passenger)
{
    // 停止文件监控，避免重复加载
    stopFileMonitoring();
    flightManager->stopFileMonitoring();
    
    // 获取文件锁管理器
    FileLockManager* lockManager = FileLockManager::getInstance();
    
    // 执行购票操作
    Ticket* ticket = purchaseTicketInternal(flightID, passenger);
    
    if (ticket) {
        bool saveSuccess = true;
        
        // 保存航班数据
        if (lockManager->acquireLock("flights.txt", 1000)) {
            if (!flightManager->saveToFile("flights.txt")) {
                saveSuccess = false;
                qDebug() << "保存航班数据失败";
            }
            lockManager->releaseLock("flights.txt");
        }
        
        // 保存票务数据
        if (lockManager->acquireLock("tickets.txt", 1000)) {
            if (!saveToFile("tickets.txt")) {
                saveSuccess = false;
                qDebug() << "保存票务数据失败";
            }
            lockManager->releaseLock("tickets.txt");
        }
        
        if (!saveSuccess) {
            // 如果保存失败，回滚购票操作
            ticket->flight->availableSeats++;
            tickets.remove(QString::fromStdString(ticket->ticketID));
            delete ticket;
            ticket = nullptr;
            qDebug() << "购票操作已回滚";
        }
    }
    
    // 重新启动文件监控
    QTimer::singleShot(500, this, [this]() {
        startFileMonitoring();
        flightManager->startFileMonitoring();
    });
    
    return ticket;
}

Ticket* TicketManager::purchaseTicketInternal(const QString& flightID, const QString& passenger)
{
    try {
        Flight* flight = flightManager->findFlight(flightID);
        if (!flight) {
            qDebug() << "找不到航班：" << flightID;
            return nullptr;
        }
        
        if (!isValidPurchase(flight, passenger)) {
            qDebug() << "购票验证失败：" << passenger << "航班：" << flightID << "余票：" << flight->availableSeats;
            return nullptr;
        }

        Ticket* ticket = createTicket(flight, passenger);
        if (!ticket) {
            qDebug() << "创建票务失败";
            return nullptr;
        }
        
        flight->availableSeats--;
        tickets[QString::fromStdString(ticket->ticketID)] = ticket;
        
        qDebug() << "购票成功：" << passenger << "航班：" << flightID << "票号：" << QString::fromStdString(ticket->ticketID);
        
        // 直接发射信号，不使用延迟
        emit ticketPurchased(ticket);
        
        return ticket;
    } catch (const std::exception& e) {
        qDebug() << "购票过程中发生异常：" << e.what();
        return nullptr;
    } catch (...) {
        qDebug() << "购票过程中发生未知异常";
        return nullptr;
    }
}

bool TicketManager::refundTicket(const QString& ticketID)
{
    // 停止文件监控
    stopFileMonitoring();
    flightManager->stopFileMonitoring();
    
    Ticket* ticket = tickets.value(ticketID);
    if (!ticket) {
        // 重新启动文件监控
        QTimer::singleShot(500, this, [this]() {
            startFileMonitoring();
            flightManager->startFileMonitoring();
        });
        return false;
    }

    Flight* flight = ticket->flight;
    flight->availableSeats++;
    tickets.remove(ticketID);
    delete ticket;
    
    // 保存更改
    FileLockManager* lockManager = FileLockManager::getInstance();
    bool saveSuccess = true;
    
    // 保存航班数据
    if (lockManager->acquireLock("flights.txt", 1000)) {
        if (!flightManager->saveToFile("flights.txt")) {
            saveSuccess = false;
        }
        lockManager->releaseLock("flights.txt");
    }
    
    // 保存票务数据
    if (lockManager->acquireLock("tickets.txt", 1000)) {
        if (!saveToFile("tickets.txt")) {
            saveSuccess = false;
        }
        lockManager->releaseLock("tickets.txt");
    }
    
    if (saveSuccess) {
        emit ticketRefunded(ticketID);
    }
    
    // 重新启动文件监控
    QTimer::singleShot(500, this, [this]() {
        startFileMonitoring();
        flightManager->startFileMonitoring();
    });
    
    return saveSuccess;
}

void TicketManager::addReservation(const QString& flightID, const QString& passenger)
{
    Reservation res;
    res.flightID = flightID;
    res.passenger = passenger;
    res.timestamp = QDateTime::currentDateTime();
    reservations.enqueue(res);
}

void TicketManager::processReservations()
{
    while (!reservations.isEmpty()) {
        Reservation res = reservations.dequeue();

        Flight* flight = flightManager->findFlight(res.flightID);
        if (flight && flight->availableSeats > 0) {
            Ticket* ticket = purchaseTicketInternal(res.flightID, res.passenger);
            if (ticket) {
                emit reservationFulfilled(res.passenger, res.flightID);
            }
        }
    }
}

Ticket* TicketManager::findTicketByID(const QString& ticketID)
{
    Ticket* ticket = tickets.value(ticketID);
    if (ticket) {
        emit ticketFound(ticket);
    }
    return ticket;
}

QList<Ticket*> TicketManager::findTicketsByPassenger(const QString& passengerName)
{
    QList<Ticket*> result;
    for (Ticket* ticket : tickets) {
        if (ticket->getPassengerName() == passengerName) {
            result.append(ticket);
        }
    }
    emit ticketsFound(result);
    return result;
}

QList<Ticket*> TicketManager::findTicketsByFlight(const QString& flightID)
{
    QList<Ticket*> result;
    for (auto it = tickets.begin(); it != tickets.end(); ++it) {
        Ticket* ticket = it.value();
        if (ticket && ticket->flight && 
            QString::fromStdString(ticket->flight->flightID) == flightID) {
            result.append(ticket);
        }
    }
    emit ticketsFound(result);
    return result;
}

QList<Ticket*> TicketManager::getAllTickets()
{
    QList<Ticket*> result;
    for (auto it = tickets.begin(); it != tickets.end(); ++it) {
        if (it.value()) {
            result.append(it.value());
        }
    }
    return result;
}

bool TicketManager::isValidPurchase(Flight* flight, const QString& passenger)
{
    if (!flight || passenger.isEmpty() || flight->availableSeats <= 0) {
        return false;
    }
    
    // 检查是否已经购买过同一航班的票
    for (auto it = tickets.begin(); it != tickets.end(); ++it) {
        Ticket* existingTicket = it.value();
        if (existingTicket && 
            existingTicket->flight == flight && 
            QString::fromStdString(existingTicket->passengerName) == passenger) {
            return false;
        }
    }
    
    return true;
}

Ticket* TicketManager::createTicket(Flight* flight, const QString& passenger)
{
    if (!flight) return nullptr;
    
    try {
        string ticketID = Ticket::generateTicketID();
        Ticket* ticket = new Ticket(ticketID, passenger.toStdString(), flight);
        ticket->purchaseTime = QDateTime::currentDateTime();
        return ticket;
    } catch (const std::exception& e) {
        qDebug() << "创建票务时发生异常：" << e.what();
        return nullptr;
    } catch (...) {
        qDebug() << "创建票务时发生未知异常";
        return nullptr;
    }
}

void TicketManager::addTicket(Ticket* ticket)
{
    if (ticket) {
        tickets[QString::fromStdString(ticket->ticketID)] = ticket;
    }
}

void TicketManager::clearAllData()
{
    for (Ticket* ticket : tickets) {
        delete ticket;
    }
    tickets.clear();
    reservations.clear();
}

// 文件读写功能实现
bool TicketManager::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    clearAllData();
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;
        
        QStringList parts = line.split(',');
        if (parts.size() < 4) continue;
        
        QString ticketID = parts[0];
        QString flightID = parts[1];
        QString passengerName = parts[2];
        QString purchaseTimeStr = parts[3];
        
        // 查找对应的航班
        Flight* flight = flightManager->findFlight(flightID);
        if (!flight) continue;
        
        // 创建票务记录
        Ticket* ticket = new Ticket(ticketID.toStdString(), passengerName.toStdString(), flight);
        ticket->purchaseTime = QDateTime::fromString(purchaseTimeStr, "yyyy-MM-dd hh:mm:ss");
        
        tickets[ticketID] = ticket;
    }
    return true;
}

bool TicketManager::saveToFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法打开票务文件进行写入：" << filename;
        return false;
    }

    try {
        QTextStream out(&file);
        int savedCount = 0;
        
        for (auto it = tickets.begin(); it != tickets.end(); ++it) {
            Ticket* ticket = it.value();
            if (!ticket || !ticket->flight) continue;
            
            try {
                QString ticketID = QString::fromStdString(ticket->ticketID);
                QString flightID = QString::fromStdString(ticket->flight->flightID);
                QString passengerName = QString::fromStdString(ticket->passengerName);
                QString purchaseTime = ticket->purchaseTime.toString("yyyy-MM-dd hh:mm:ss");
                
                if (!ticketID.isEmpty() && !flightID.isEmpty() && !passengerName.isEmpty()) {
                    out << ticketID << ","
                        << flightID << ","
                        << passengerName << ","
                        << purchaseTime << "\n";
                    savedCount++;
                }
            } catch (...) {
                qDebug() << "跳过有问题的票务记录";
                continue;
            }
        }
        
        file.flush();
        file.close();
        qDebug() << "票务数据保存完成，实际保存" << savedCount << "张票";
        return true;
    } catch (const std::exception& e) {
        qDebug() << "保存票务数据时发生异常：" << e.what();
        file.close();
        return false;
    } catch (...) {
        qDebug() << "保存票务数据时发生未知异常";
        file.close();
        return false;
    }
}

void TicketManager::clearReservations()
{
    reservations.clear();
}

// 获取特定航班的所有乘客
QList<Ticket*> TicketManager::getTicketsByFlightID(const QString& flightID)
{
    return getTicketsByFlightIDInternal(flightID);
}

// 内部无锁版本
QList<Ticket*> TicketManager::getTicketsByFlightIDInternal(const QString& flightID)
{
    QList<Ticket*> result;
    
    for (auto it = tickets.begin(); it != tickets.end(); ++it) {
        Ticket* ticket = it.value();
        if (ticket && ticket->flight && 
            QString::fromStdString(ticket->flight->flightID) == flightID) {
            result.append(ticket);
        }
    }
    
    return result;
}

void TicketManager::processReservation()
{
    if (reservations.isEmpty()) return;
    
    Reservation res = reservations.dequeue();
    
    // 尝试为这个预约分配票
    Ticket* ticket = purchaseTicketInternal(res.flightID, res.passenger);
    if (ticket) {
        emit ticketPurchased(ticket);
    }
}

// ==================== RouteGraphManager实现 ====================
RouteGraphManager::RouteGraphManager(FlightManager* flightMgr, QObject *parent)
    : QObject(parent), flightManager(flightMgr)
{
    rebuildGraph();
}

void RouteGraphManager::rebuildGraph()
{
    // 清理旧图
    for (CityNode* node : cityMap) {
        delete node;
    }
    cityMap.clear();

    // 创建新图
    const QList<Flight*>& flights = flightManager->getAllFlights();
    for (Flight* flight : flights) {
        QString fromCity = flight->getFromCity();
        QString toCity = flight->getToCity();

        // 获取或创建城市节点
        CityNode* fromNode = cityMap.value(fromCity);
        if (!fromNode) {
            fromNode = new CityNode();
            fromNode->cityName = fromCity.toStdString();
            cityMap[fromCity] = fromNode;
        }

        CityNode* toNode = cityMap.value(toCity);
        if (!toNode) {
            toNode = new CityNode();
            toNode->cityName = toCity.toStdString();
            cityMap[toCity] = toNode;
        }

        // 创建边
        RouteEdge* edge = new RouteEdge();
        edge->fromCity = fromNode;
        edge->toCity = toNode;
        edge->flight = flight;
        edge->price = flight->price;
        edge->duration = flight->departureTime.secsTo(flight->arrivalTime) / 60; // 转换为分钟

        fromNode->edges.push_back(edge);
    }
}

QList<Flight*> RouteGraphManager::findShortestPath(const QString& fromCity, const QString& toCity, SortCriteria criteria)
{
    if (!isValidCities(fromCity, toCity)) return QList<Flight*>();

    CityNode* startNode = cityMap.value(fromCity);
    QHash<CityNode*, int> dist;
    QHash<CityNode*, RouteEdge*> prev;
    QList<CityNode*> unvisited = cityMap.values();

    initializeDijkstra(fromCity, dist);

    while (!unvisited.isEmpty()) {
        CityNode* current = findMinDistanceNode(unvisited, dist);
        if (!current || dist[current] == INT_MAX) break;

        unvisited.removeOne(current);
        updateNeighbors(current, dist, prev, criteria);
    }

    return reconstructPath(toCity, prev);
}

bool RouteGraphManager::isValidCities(const QString& fromCity, const QString& toCity)
{
    return !fromCity.isEmpty() && !toCity.isEmpty() &&
           cityMap.contains(fromCity) && cityMap.contains(toCity);
}

void RouteGraphManager::initializeDijkstra(const QString& fromCity, QHash<CityNode*, int>& dist)
{
    for (CityNode* node : cityMap) {
        dist[node] = INT_MAX;
    }
    dist[cityMap.value(fromCity)] = 0;
}

CityNode* RouteGraphManager::findMinDistanceNode(const QList<CityNode*>& nodes, const QHash<CityNode*, int>& dist)
{
    CityNode* minNode = nullptr;
    int minDist = INT_MAX;

    for (CityNode* node : nodes) {
        if (dist[node] < minDist) {
            minDist = dist[node];
            minNode = node;
        }
    }
    return minNode;
}

void RouteGraphManager::updateNeighbors(CityNode* current, QHash<CityNode*, int>& dist,
                                      QHash<CityNode*, RouteEdge*>& prev, SortCriteria criteria)
{
    for (RouteEdge* edge : current->edges) {
        CityNode* neighbor = edge->toCity;
        int weight = (criteria == SortCriteria::Price) ? edge->price : edge->duration;
        int newDist = dist[current] + weight;

        if (newDist < dist[neighbor]) {
            dist[neighbor] = newDist;
            prev[neighbor] = edge;
        }
    }
}

QList<Flight*> RouteGraphManager::reconstructPath(const QString& toCity, const QHash<CityNode*, RouteEdge*>& prev)
{
    QList<Flight*> path;
    CityNode* current = cityMap.value(toCity);

    while (prev.contains(current)) {
        RouteEdge* edge = prev[current];
        path.prepend(edge->flight);
        current = edge->fromCity;
    }

    return path;
}

// ==================== NotificationManager实现 ====================
NotificationManager::NotificationManager(FlightManager* flightMgr,
                                       RouteGraphManager* routeMgr,
                                       QObject *parent)
    : QObject(parent)
    , flightManager(flightMgr)
    , routeGraphManager(routeMgr)
    , monitorTimer(new QTimer(this))
{
    connect(monitorTimer, &QTimer::timeout, this, [this]() {
        // 检查所有航班状态
        for (Flight* flight : flightManager->getAllFlights()) {
            if (flight->status == FlightStatus::Delayed) {
                sendNotification(QString("航班 %1 延误").arg(flight->getFlightID()));
            } else if (flight->status == FlightStatus::Cancelled) {
                sendNotification(QString("航班 %1 取消").arg(flight->getFlightID()));
            }
        }
    });
}

void NotificationManager::startMonitoring()
{
    monitorTimer->start(60000); // 每分钟检查一次
}

void NotificationManager::sendNotification(const QString& message)
{
    emit notificationGenerated(message);
}

void NotificationManager::onFlightStatusChanged(QString flightID, FlightStatus status)
{
    Flight* flight = flightManager->findFlight(flightID);
    if (!flight) return;

    QString message;
    switch (status) {
        case FlightStatus::Delayed:
            message = QString("航班 %1 延误").arg(flightID);
            break;
        case FlightStatus::Cancelled:
            message = QString("航班 %1 取消").arg(flightID);
            break;
        default:
            return;
    }
    sendNotification(message);
}

// TicketManager的文件监控相关函数实现
void TicketManager::setDataFilePath(const QString& filePath)
{
    dataFilePath = filePath;
}

void TicketManager::startFileMonitoring()
{
    if (!fileWatcher) {
        fileWatcher = new QFileSystemWatcher(this);
        connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &TicketManager::onFileChanged);
    }

    if (!dataFilePath.isEmpty() && QFile::exists(dataFilePath)) {
        if (!fileWatcher->files().contains(dataFilePath)) {
            fileWatcher->addPath(dataFilePath);
        }
    }
}

void TicketManager::stopFileMonitoring()
{
    if (fileWatcher && !dataFilePath.isEmpty()) {
        fileWatcher->removePath(dataFilePath);
    }
}

void TicketManager::onFileChanged(const QString& path)
{
    if (path == dataFilePath) {
        if (!reloadTimer) {
            reloadTimer = new QTimer(this);
            reloadTimer->setSingleShot(true);
            reloadTimer->setInterval(500); // 500ms延迟
            connect(reloadTimer, &QTimer::timeout, this, &TicketManager::delayedReload);
        }
        reloadTimer->start();
    }
}

void TicketManager::delayedReload()
{
    stopFileMonitoring();
    bool changed = false;
    if (loadFromFile(dataFilePath)) {
        changed = true;
    }
    QTimer::singleShot(500, this, [this]() {
        startFileMonitoring();
    });
}
