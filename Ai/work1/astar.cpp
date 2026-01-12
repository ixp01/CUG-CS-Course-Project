/**
 * A* Search Algorithm - A*搜索算法实现
 * 
 * 算法原理：
 * - 结合了UCS的最优性和贪婪算法的效率
 * - 使用评估函数 f(n) = g(n) + h(n) 来选择节点
 *   - g(n): 从起点到当前节点的实际代价
 *   - h(n): 从当前节点到目标的启发式估计
 * - 如果启发式函数是可采纳的(admissible)，则保证找到最优解
 * - 如果启发式函数是一致的(consistent)，则算法是最优效率的
 * - 时间复杂度：O(b^d)，其中d是最优解的深度
 * - 空间复杂度：O(b^d)
 */

#include "astar.h"
#include "graph.h"
#include <queue>
#include <QSet>

/**
 * 比较器：用于优先队列按f(n) = g(n) + h(n)排序
 * f值小的节点优先级高（更有希望通向最优解）
 */
struct CompareAStar {
    bool operator()(const Node &a, const Node &b) const {
        // 比较f(n) = g(n) + h(n)的值
        return (a.cost + a.heuristic) > (b.cost + b.heuristic);  // 小顶堆，f值小的在前
    }
};

/**
 * A*算法的核心实现
 * @param start 起始城市名称
 * @param goal 目标城市名称
 * @return PathResult 包含最优路径、总代价和访问节点数
 */
PathResult AStar::findPath(const QString &start, const QString &goal) {
    // 初始化优先队列（按f(n) = g(n) + h(n)排序的小顶堆）
    std::priority_queue<Node, std::vector<Node>, CompareAStar> frontier;
    
    // 已访问节点集合，避免重复访问
    QSet<QString> visited;
    
    // 将起始节点加入队列
    frontier.push({start, 0, HEURISTIC[start], {start}});
    
    // 统计访问节点数量（算法效率指标）
    int visitCount = 0;
    
    // 主搜索循环：当队列非空时继续搜索
    while (!frontier.empty()) {
        // 取出f(n)值最小的节点进行扩展
        Node current = frontier.top(); 
        frontier.pop();
        visitCount++;
        
        // 输出当前扩展的节点信息到控制台
        emit logMessage(QString("扩展节点: %1, f(n)=g+h: %2, g(n): %3, h(n): %4")
            .arg(current.name)
            .arg(current.cost + current.heuristic)  // f(n) = g(n) + h(n)
            .arg(current.cost)                      // g(n): 实际代价
            .arg(current.heuristic));               // h(n): 启发式估计
        
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
                next.cost = current.cost + neighbor.second;    // 更新g(n): 累计实际代价
                next.heuristic = HEURISTIC[neighbor.first];   // h(n): 邻居城市的启发式值
                next.path = current.path;                     // 复制当前路径
                next.path.append(neighbor.first);            // 添加邻居到路径
                
                // 输出邻居节点信息到控制台
                emit logMessage(QString("  邻居节点加入队列: %1, f(n)=g+h: %2, g(n): %3, h(n): %4")
                    .arg(next.name)
                    .arg(next.cost + next.heuristic)  // f(n) = g(n) + h(n)
                    .arg(next.cost)                   // g(n): 实际代价
                    .arg(next.heuristic));            // h(n): 启发式估计
                
                // 将邻居节点加入优先队列（按f(n)值排序）
                frontier.push(next);
            }
        }
    }
    
    // 搜索失败：队列为空但未找到目标
    return {{}, -1, visitCount};
}
