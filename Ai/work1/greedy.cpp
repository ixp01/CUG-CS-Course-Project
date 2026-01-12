/**
 * Greedy Best-First Search - 贪婪最佳优先搜索算法实现
 * 
 * 算法原理：
 * - 总是扩展启发式函数值最小的节点（最接近目标）
 * - 只使用启发式函数h(n)进行选择，忽略已经花费的代价g(n)
 * - 不保证找到最优解，但通常能快速找到较好的解
 * - 时间复杂度：O(b^m)，其中m是搜索空间的最大深度
 * - 空间复杂度：O(b^m)
 */

#include "greedy.h"
#include "graph.h"
#include <queue>
#include <QSet>

/**
 * 比较器：用于优先队列按启发式值排序
 * 启发式值小的节点优先级高（更接近目标）
 */
struct CompareHeuristic {
    bool operator()(const Node &a, const Node &b) const {
        return a.heuristic > b.heuristic;  // 小顶堆，启发式值小的在前
    }
};

/**
 * 贪婪算法的核心实现
 * @param start 起始城市名称
 * @param goal 目标城市名称
 * @return PathResult 包含找到的路径、总代价和访问节点数
 */
PathResult Greedy::findPath(const QString &start, const QString &goal) {
    // 初始化优先队列（按启发式值排序的小顶堆）
    std::priority_queue<Node, std::vector<Node>, CompareHeuristic> frontier;
    
    // 已访问节点集合，避免重复访问
    QSet<QString> visited;
    
    // 将起始节点加入队列（使用起始城市的启发式值）
    frontier.push({start, 0, HEURISTIC[start], {start}});
    
    // 统计访问节点数量（算法效率指标）
    int visitCount = 0;
    
    // 主搜索循环：当队列非空时继续搜索
    while (!frontier.empty()) {
        // 取出启发式值最小的节点进行扩展
        Node current = frontier.top(); 
        frontier.pop();
        visitCount++;
        
        // 输出当前扩展的节点信息到控制台
        emit logMessage(QString("扩展节点: %1, 启发值: %2, 当前总代价: %3")
            .arg(current.name).arg(current.heuristic).arg(current.cost));
        
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
                next.cost = current.cost + neighbor.second;    // 累计实际代价
                next.heuristic = HEURISTIC[neighbor.first];   // 邻居城市的启发式值
                next.path = current.path;                     // 复制当前路径
                next.path.append(neighbor.first);            // 添加邻居到路径
                
                // 输出邻居节点信息到控制台
                emit logMessage(QString("  邻居节点加入队列: %1, 启发值: %2, 路径总代价: %3")
                    .arg(next.name)
                    .arg(next.heuristic)
                    .arg(next.cost));
                
                // 将邻居节点加入优先队列（按启发式值排序）
                frontier.push(next);
            }
        }
    }
    
    // 搜索失败：队列为空但未找到目标
    return {{}, -1, visitCount};
}
