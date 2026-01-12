// * UCS (Uniform Cost Search) - 代价一致搜索算法实现
 // *
 // * 算法原理：
 // * - 总是扩展路径代价最小的节点
 // * - 使用优先队列按照累计代价g(n)排序
 // * - 保证找到最优解（最短路径）
 // * - 时间复杂度：O(b^(1+C*/ε))，其中C*是最优解代价，ε是步长的最小值


#include "ucs.h"
#include "graph.h"
#include <queue>
#include <QSet>

/**
 * 比较器：用于优先队列按代价排序
 * 代价小的节点优先级高（在队列前面）
 */
struct CompareCost {
    bool operator()(const Node &a, const Node &b) const {
        return a.cost > b.cost;  // 小顶堆，代价小的在前
    }
};

/**
 * UCS算法的核心实现
 * @param start 起始城市名称
 * @param goal 目标城市名称
 * @return PathResult 包含最优路径、总代价和访问节点数
 */
PathResult UCS::findPath(const QString &start, const QString &goal) {
    // 初始化优先队列（按代价排序的小顶堆）
    std::priority_queue<Node, std::vector<Node>, CompareCost> frontier;
    
    // 已访问节点集合，避免重复访问
    QSet<QString> visited;
    
    // 将起始节点加入队列
    frontier.push({start, 0, 0, {start}});
    
    // 统计访问节点数量（算法效率指标）
    int visitCount = 0;
    
    // 主搜索循环：当队列非空时继续搜索
    while (!frontier.empty()) {
        // 取出代价最小的节点进行扩展
        Node current = frontier.top(); 
        frontier.pop();
        visitCount++;
        
        // 输出当前扩展的节点信息到控制台
        emit logMessage(QString("扩展节点: %1, 当前总代价: %2").arg(current.name).arg(current.cost));
        
        // 跳过已访问的节点（图搜索版本，避免重复访问）
        if (visited.contains(current.name)) continue;
        
        // 标记当前节点为已访问
        visited.insert(current.name);
        
        // 目标测试：检查是否到达目标节点
        if (current.name == goal) {
            emit logMessage(QString("找到目标节点: %1，总路径: %2，总代价: %3")
                .arg(goal)
                .arg(current.path.join(" -> "))
                .arg(current.cost));
            return {current.path, current.cost, visitCount};
        }
        
        // 扩展当前节点：将所有未访问的邻居节点加入队列
        for (const auto &neighbor : ROMANIA_MAP[current.name]) {
            if (!visited.contains(neighbor.first)) {
                // 创建邻居节点
                Node next;
                next.name = neighbor.first;                    // 邻居城市名
                next.cost = current.cost + neighbor.second;    // 累计代价 = 当前代价 + 边权重
                next.heuristic = 0;                           // UCS不使用启发式信息
                next.path = current.path;                     // 复制当前路径
                next.path.append(neighbor.first);            // 添加邻居到路径
                
                // 输出邻居节点信息到控制台
                emit logMessage(QString("  邻居节点加入队列: %1, 路径总代价: %2")
                    .arg(next.name).arg(next.cost));
                
                // 将邻居节点加入优先队列
                frontier.push(next);
            }
        }
    }
    
    // 搜索失败：队列为空但未找到目标
    return {{}, -1, visitCount};
}
