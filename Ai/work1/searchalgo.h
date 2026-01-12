#ifndef SEARCHALGO_H
#define SEARCHALGO_H
#include <QObject>
#include <QString>
#include <QList>
#include <QPair>

/**
 * 路径搜索结果结构体
 * 用于存储搜索算法返回的完整结果信息
 */
struct PathResult {
    QList<QString> path;    // 从起点到终点的完整路径（城市名列表）
    int totalCost = 0;      // 路径的总代价（总距离）
    int visitedNodes = 0;   // 搜索过程中访问的节点数量（算法效率指标）
};

/**
 * 搜索节点结构体
 * 表示搜索过程中的一个状态节点
 */
struct Node {
    QString name;           // 当前节点的城市名称
    int cost;              // 从起点到当前节点的累计代价 g(n)
    int heuristic;         // 从当前节点到目标的启发式估计 h(n)
    QList<QString> path;   // 从起点到当前节点的路径追踪
    
    // 重载小于运算符，用于优先队列排序（UCS按cost排序）
    bool operator<(const Node &other) const { return cost > other.cost; }
};

/**
 * 搜索算法抽象基类
 * 定义了所有搜索算法的通用接口和信号机制
 */
class SearchAlgorithm : public QObject {
    Q_OBJECT
public:
    /**
     * 构造函数
     * @param parent 父对象指针，用于Qt的对象管理
     */
    explicit SearchAlgorithm(QObject *parent = nullptr) : QObject(parent) {}
    
    /**
     * 纯虚函数：查找从起点到目标的路径
     * 每个具体算法都必须实现这个函数
     * @param start 起点城市名称
     * @param goal 目标城市名称
     * @return PathResult 包含路径、代价和访问节点数的结果
     */
    virtual PathResult findPath(const QString &start, const QString &goal) = 0;

signals:
    /**
     * 日志消息信号
     * 算法执行过程中发出的消息，用于在UI界面显示搜索过程
     * @param msg 要显示的消息内容
     */
    void logMessage(const QString &msg);
};

#endif // SEARCHALGO_H 