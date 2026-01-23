#ifndef FLIGHTGRAPH_H
#define FLIGHTGRAPH_H

#include "data.h"
#include <QMap>
#include <QList>
#include <QPair>
#include <QString>
#include <QVector>
#include <QDateTime>

// 航班边结构（图的边）
struct FlightEdge {
    QString destination;    // 目标城市
    QString flightNumber;   // 航班号
    double price;          // 价格
    QDateTime departureTime; // 起飞时间
    QDateTime arrivalTime;  // 到达时间
    
    FlightEdge() : price(0.0) {}
    
    FlightEdge(const QString& dest, const QString& flight, double p, 
               const QDateTime& dep, const QDateTime& arr)
        : destination(dest), flightNumber(flight), price(p), 
          departureTime(dep), arrivalTime(arr) {}
};

// 航班图类（用于转机推荐算法）
class FlightGraph {
public:
    FlightGraph();
    ~FlightGraph();
    
    // 图构建
    void buildGraph(const QVector<Flight>& flights);
    void clearGraph();
    
    // 路径查找算法
    QVector<TransferPlan> findCheapestRoutes(const QString& from, const QString& to, 
                                           const QDate& date, int maxTransfers = 2);
    QVector<TransferPlan> findFastestRoutes(const QString& from, const QString& to, 
                                          const QDate& date, int maxTransfers = 2);
    
    // 推荐系统
    QVector<TransferPlan> recommendTransfers(const QString& from, const QString& to, 
                                           const QDate& date);
    QVector<QString> findAlternativeDestinations(const QString& from, int maxPrice = -1);
    
    // 图信息
    QVector<QString> getAllCities() const;
    int getRouteCount() const;
    bool hasDirectFlight(const QString& from, const QString& to) const;
    
    // 调试信息
    void printGraph() const;

private:
    // 邻接表：城市 -> 可达城市列表
    QMap<QString, QList<FlightEdge>> adjacencyList;
    
    // 私有算法方法
    QVector<TransferPlan> dijkstraSearch(const QString& from, const QString& to, 
                                       const QDate& date, bool byPrice = true);
    void dfsSearch(const QString& current, const QString& target, const QDate& date,
                   QVector<Flight>& currentPath, QVector<TransferPlan>& results,
                   double currentPrice, QDateTime lastArrival, int maxTransfers);
    
    // 工具方法
    bool isValidConnection(const QDateTime& arrival, const QDateTime& departure) const;
    double calculateTotalPrice(const QVector<Flight>& flights) const;
    QDateTime calculateTotalTime(const QVector<Flight>& flights) const;
    TransferPlan createTransferPlan(const QVector<Flight>& flights) const;
};

#endif // FLIGHTGRAPH_H 