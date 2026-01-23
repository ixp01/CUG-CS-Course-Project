#include "../include/flightgraph.h"
#include <QDebug>
#include <algorithm>
#include <limits>
#include <QSet>
#include <QQueue>
#include <QMap>

FlightGraph::FlightGraph() {
    // 构造函数
}

FlightGraph::~FlightGraph() {
    // 析构函数
    clearGraph();
}

void FlightGraph::buildGraph(const QVector<Flight>& flights) {
    // 清空现有图
    clearGraph();
    
    // 遍历所有航班，构建图结构
    for (const Flight& flight : flights) {
        // 创建航班边
        FlightEdge edge(flight.arrivalCity, flight.flightNumber, flight.price,
                       flight.departureTime, flight.arrivalTime);
        
        // 添加边到邻接表
        adjacencyList[flight.departureCity].append(edge);
        
        // 确保目的地城市也在邻接表中
        if (!adjacencyList.contains(flight.arrivalCity)) {
            adjacencyList[flight.arrivalCity] = QList<FlightEdge>();
        }
        
        // 处理经停站点
        QString currentCity = flight.departureCity;
        QDateTime currentTime = flight.departureTime;
        
        for (const QString& stopover : flight.stopovers) {
            // 估算到经停地的时间和价格（简化处理）
            QDateTime stopoverTime = currentTime.addSecs(3600); // 假设每段1小时
            double segmentPrice = flight.price / (flight.stopovers.size() + 1);
            
            FlightEdge stopoverEdge(stopover, flight.flightNumber + "_" + stopover,
                                  segmentPrice, currentTime, stopoverTime);
            adjacencyList[currentCity].append(stopoverEdge);
            
            // 确保经停城市在邻接表中
            if (!adjacencyList.contains(stopover)) {
                adjacencyList[stopover] = QList<FlightEdge>();
            }
            
            currentCity = stopover;
            currentTime = stopoverTime.addSecs(1800); // 经停30分钟
        }
        
        // 从最后一个经停地到目的地
        if (!flight.stopovers.isEmpty()) {
            double segmentPrice = flight.price / (flight.stopovers.size() + 1);
            FlightEdge finalEdge(flight.arrivalCity, flight.flightNumber + "_final",
                               segmentPrice, currentTime, flight.arrivalTime);
            adjacencyList[currentCity].append(finalEdge);
        }
    }
    
    qDebug() << QString("图构建完成：%1个城市，%2个航班")
                .arg(adjacencyList.size()).arg(flights.size());
}

void FlightGraph::clearGraph() {
    adjacencyList.clear();
}

QVector<TransferPlan> FlightGraph::findCheapestRoutes(const QString& from, const QString& to, 
                                                     const QDate& date, int maxTransfers) {
    return dijkstraSearch(from, to, date, true);
}

QVector<TransferPlan> FlightGraph::findFastestRoutes(const QString& from, const QString& to, 
                                                    const QDate& date, int maxTransfers) {
    return dijkstraSearch(from, to, date, false);
}

QVector<TransferPlan> FlightGraph::recommendTransfers(const QString& from, const QString& to, 
                                                     const QDate& date) {
    QVector<TransferPlan> results;
    
    // 1. 查找直飞航班
    if (adjacencyList.contains(from)) {
        const QList<FlightEdge>& edges = adjacencyList[from];
        for (const FlightEdge& edge : edges) {
            if (edge.destination == to && edge.departureTime.date() == date) {
                TransferPlan plan;
                
                // 创建Flight对象（从FlightEdge转换）
                Flight flight;
                flight.flightNumber = edge.flightNumber;
                flight.departureCity = from;
                flight.arrivalCity = edge.destination;
                flight.departureTime = edge.departureTime;
                flight.arrivalTime = edge.arrivalTime;
                flight.price = edge.price;
                
                plan.flights.append(flight);
                plan.totalPrice = edge.price;
                plan.departureTime = edge.departureTime;
                plan.arrivalTime = edge.arrivalTime;
                plan.transferCount = 0;
                results.append(plan);
            }
        }
    }
    
    // 2-3. 转机查找 - 使用DFS搜索最多2次转机的路径
    QVector<Flight> currentPath;
    QVector<TransferPlan> transferResults;
    dfsSearch(from, to, date, currentPath, transferResults, 0.0, QDateTime(), 2);
    
    // 将转机结果添加到总结果中
    for (const TransferPlan& plan : transferResults) {
        if (plan.transferCount > 0) { // 只添加转机路径，直飞已经处理
            results.append(plan);
        }
    }
    
    // 4. 按综合评分排序（价格权重0.6，时间权重0.4）
    std::sort(results.begin(), results.end(), [](const TransferPlan& a, const TransferPlan& b) {
        double scoreA = a.totalPrice * 0.6 + a.departureTime.secsTo(a.arrivalTime) / 3600.0 * 0.4;
        double scoreB = b.totalPrice * 0.6 + b.departureTime.secsTo(b.arrivalTime) / 3600.0 * 0.4;
        return scoreA < scoreB;
    });
    
    // 限制返回结果数量
    if (results.size() > 5) {
        results.resize(5);
    }
    
    return results;
}

QVector<QString> FlightGraph::findAlternativeDestinations(const QString& from, int maxPrice) {
    QVector<QString> destinations;
    
    if (!adjacencyList.contains(from)) {
        return destinations;
    }
    
    const QList<FlightEdge>& edges = adjacencyList[from];
    for (const FlightEdge& edge : edges) {
        if (maxPrice < 0 || edge.price <= maxPrice) {
            if (!destinations.contains(edge.destination)) {
                destinations.append(edge.destination);
            }
        }
    }
    
    // 按目的地名称排序
    std::sort(destinations.begin(), destinations.end());
    
    return destinations;
}

QVector<QString> FlightGraph::getAllCities() const {
    return adjacencyList.keys().toVector();
}

int FlightGraph::getRouteCount() const {
    int count = 0;
    for (const auto& edges : adjacencyList) {
        count += edges.size();
    }
    return count;
}

bool FlightGraph::hasDirectFlight(const QString& from, const QString& to) const {
    if (!adjacencyList.contains(from)) {
        return false;
    }
    
    const QList<FlightEdge>& edges = adjacencyList[from];
    for (const FlightEdge& edge : edges) {
        if (edge.destination == to) {
            return true;
        }
    }
    return false;
}

void FlightGraph::printGraph() const {
    qDebug() << "=== 航班图结构 ===";
    qDebug() << QString("城市总数: %1, 航线总数: %2").arg(adjacencyList.size()).arg(getRouteCount());
    
    for (auto it = adjacencyList.begin(); it != adjacencyList.end(); ++it) {
        const QString& city = it.key();
        const QList<FlightEdge>& edges = it.value();
        
        qDebug() << QString("城市 %1 -> %2 条航线:").arg(city).arg(edges.size());
        for (const FlightEdge& edge : edges) {
            qDebug() << QString("  -> %1 (航班: %2, 价格: ¥%3, 起飞: %4)")
                        .arg(edge.destination)
                        .arg(edge.flightNumber)
                        .arg(edge.price, 0, 'f', 0)
                        .arg(edge.departureTime.toString("yyyy-MM-dd hh:mm"));
        }
    }
    qDebug() << "==================";
}

// 私有方法实现
QVector<TransferPlan> FlightGraph::dijkstraSearch(const QString& from, const QString& to, 
                                                 const QDate& date, bool byPrice) {
    QVector<TransferPlan> results;
    
    if (!adjacencyList.contains(from) || !adjacencyList.contains(to)) {
        return results;
    }
    
    // 使用简化的BFS搜索直飞和一次转机
    QQueue<QPair<QString, QVector<Flight>>> queue;
    QSet<QString> visited;
    
    // 首先查找直飞
    if (adjacencyList.contains(from)) {
        const QList<FlightEdge>& edges = adjacencyList[from];
        for (const FlightEdge& edge : edges) {
            if (edge.destination == to && edge.departureTime.date() == date) {
                Flight flight;
                flight.flightNumber = edge.flightNumber;
                flight.departureCity = from;
                flight.arrivalCity = edge.destination;
                flight.departureTime = edge.departureTime;
                flight.arrivalTime = edge.arrivalTime;
                flight.price = edge.price;
                flight.status = "Scheduled";
                
                QVector<Flight> path;
                path.append(flight);
                results.append(createTransferPlan(path));
            }
        }
    }
    
    // 查找一次转机的路径
    if (adjacencyList.contains(from)) {
        const QList<FlightEdge>& firstEdges = adjacencyList[from];
        for (const FlightEdge& firstEdge : firstEdges) {
            if (firstEdge.departureTime.date() != date) continue;
            
            QString transferCity = firstEdge.destination;
            if (transferCity == to) continue; // 直飞已经处理
            
            if (adjacencyList.contains(transferCity)) {
                const QList<FlightEdge>& secondEdges = adjacencyList[transferCity];
                for (const FlightEdge& secondEdge : secondEdges) {
                    if (secondEdge.destination == to && 
                        isValidConnection(firstEdge.arrivalTime, secondEdge.departureTime)) {
                        
                        // 创建第一段航班
                        Flight firstFlight;
                        firstFlight.flightNumber = firstEdge.flightNumber;
                        firstFlight.departureCity = from;
                        firstFlight.arrivalCity = firstEdge.destination;
                        firstFlight.departureTime = firstEdge.departureTime;
                        firstFlight.arrivalTime = firstEdge.arrivalTime;
                        firstFlight.price = firstEdge.price;
                        firstFlight.status = "Scheduled";
                        
                        // 创建第二段航班
                        Flight secondFlight;
                        secondFlight.flightNumber = secondEdge.flightNumber;
                        secondFlight.departureCity = transferCity;
                        secondFlight.arrivalCity = secondEdge.destination;
                        secondFlight.departureTime = secondEdge.departureTime;
                        secondFlight.arrivalTime = secondEdge.arrivalTime;
                        secondFlight.price = secondEdge.price;
                        secondFlight.status = "Scheduled";
                        
                        QVector<Flight> path;
                        path.append(firstFlight);
                        path.append(secondFlight);
                        results.append(createTransferPlan(path));
                    }
                }
            }
        }
    }
    
    // 按价格或时间排序
    if (byPrice) {
        std::sort(results.begin(), results.end(), [](const TransferPlan& a, const TransferPlan& b) {
            return a.totalPrice < b.totalPrice;
        });
    } else {
        std::sort(results.begin(), results.end(), [](const TransferPlan& a, const TransferPlan& b) {
            qint64 durationA = a.departureTime.secsTo(a.arrivalTime);
            qint64 durationB = b.departureTime.secsTo(b.arrivalTime);
            return durationA < durationB;
        });
    }
    
    // 限制返回结果数量
    if (results.size() > 10) {
        results.resize(10);
    }
    
    return results;
}

void FlightGraph::dfsSearch(const QString& current, const QString& target, const QDate& date,
                           QVector<Flight>& currentPath, QVector<TransferPlan>& results,
                           double currentPrice, QDateTime lastArrival, int maxTransfers) {
    // 如果到达目标城市，创建转机方案
    if (current == target && !currentPath.isEmpty()) {
        results.append(createTransferPlan(currentPath));
        return;
    }
    
    // 如果转机次数超过限制，停止搜索
    if (currentPath.size() >= maxTransfers + 1) {
        return;
    }
    
    // 搜索从当前城市出发的航班
    if (adjacencyList.contains(current)) {
        const QList<FlightEdge>& edges = adjacencyList[current];
        
        for (const FlightEdge& edge : edges) {
            // 检查日期和转机时间
            if (edge.departureTime.date() != date) continue;
            if (!currentPath.isEmpty() && !isValidConnection(lastArrival, edge.departureTime)) continue;
            
            // 避免访问已经在路径中的城市（避免循环）
            bool cityInPath = false;
            for (const Flight& flight : currentPath) {
                if (flight.arrivalCity == edge.destination) {
                    cityInPath = true;
                    break;
                }
            }
            if (cityInPath) continue;
            
            // 创建航班对象
            Flight flight;
            flight.flightNumber = edge.flightNumber;
            flight.departureCity = current;
            flight.arrivalCity = edge.destination;
            flight.departureTime = edge.departureTime;
            flight.arrivalTime = edge.arrivalTime;
            flight.price = edge.price;
            flight.status = "Scheduled";
            
            // 添加到路径并继续搜索
            currentPath.append(flight);
            dfsSearch(edge.destination, target, date, currentPath, results,
                     currentPrice + edge.price, edge.arrivalTime, maxTransfers);
            currentPath.removeLast(); // 回溯
        }
    }
}

bool FlightGraph::isValidConnection(const QDateTime& arrival, const QDateTime& departure) const {
    if (!arrival.isValid() || !departure.isValid()) {
        return false;
    }
    // 检查转机时间是否至少60分钟
    return arrival.secsTo(departure) >= 3600;
}

double FlightGraph::calculateTotalPrice(const QVector<Flight>& flights) const {
    double totalPrice = 0.0;
    for (const Flight& flight : flights) {
        totalPrice += flight.price;
    }
    return totalPrice;
}

QDateTime FlightGraph::calculateTotalTime(const QVector<Flight>& flights) const {
    if (flights.isEmpty()) {
        return QDateTime();
    }
    return flights.last().arrivalTime;
}

TransferPlan FlightGraph::createTransferPlan(const QVector<Flight>& flights) const {
    TransferPlan plan;
    plan.flights = flights;
    plan.totalPrice = calculateTotalPrice(flights);
    plan.transferCount = flights.size() - 1;
    
    if (!flights.isEmpty()) {
        plan.departureTime = flights.first().departureTime;
        plan.arrivalTime = flights.last().arrivalTime;
    }
    
    return plan;
} 