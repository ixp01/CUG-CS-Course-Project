#include "citygraph.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <QDateTime>
#include <QStringConverter>
#include <cmath>
#include <set>

CityGraph::CityGraph() : floydComputed(false) {}

CityGraph::~CityGraph() {}

bool CityGraph::loadFromCSV(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件:" << filename;
        return false;
    }
    
    QTextStream in(&file);
    
    // 读取第一行获取城市名称
    QString firstLine = in.readLine();
    QStringList cityList = firstLine.split(',');
    
    // 去掉第一列的"Unnamed: 0"
    cities.clear();
    for (int i = 1; i < cityList.size(); i++) {
        QString cityName = cityList[i].trimmed();
        if (!cityName.isEmpty()) {
            cities.push_back(cityName);
        }
    }
    
    int numCities = cities.size();
    adjacencyList.clear();
    adjacencyList.resize(numCities);
    
    // 读取距离矩阵
    int row = 0;
    while (!in.atEnd() && row < numCities) {
        QString line = in.readLine();
        QStringList distances = line.split(',');
        
        for (int col = 1; col < distances.size() && col <= numCities; col++) {
            QString distStr = distances[col].trimmed();
            if (!distStr.isEmpty() && distStr != "0.0") {
                bool ok;
                double distance = distStr.toDouble(&ok);
                if (ok && distance > 0) {
                    adjacencyList[row].push_back(Edge(col - 1, distance));
                }
            }
        }
        row++;
    }
    
    file.close();
    floydComputed = false;
    
    qDebug() << "成功加载" << numCities << "个城市的数据";
    return true;
}

int CityGraph::findCityIndex(const QString& cityName) const {
    for (int i = 0; i < cities.size(); i++) {
        if (cities[i] == cityName) {
            return i;
        }
    }
    return -1;
}

QString CityGraph::getCityName(int index) const {
    if (index >= 0 && index < cities.size()) {
        return cities[index];
    }
    return "";
}

int CityGraph::numCities() const {
    return cities.size();
}

const std::vector<QString>& CityGraph::getCityNames() const {
    return cities;
}

PathInfo CityGraph::dijkstra(int start, int end) const {
    int n = adjacencyList.size();
    std::vector<double> dist(n, std::numeric_limits<double>::max());
    std::vector<int> prev(n, -1);
    std::priority_queue<std::pair<double, int>, 
                       std::vector<std::pair<double, int>>, 
                       std::greater<std::pair<double, int>>> pq;
    
    dist[start] = 0;
    pq.push({0, start});
    
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        
        if (u == end) break;
        if (d > dist[u]) continue;
        
        for (const Edge& edge : adjacencyList[u]) {
            double newDist = dist[u] + edge.distance;
            if (newDist < dist[edge.dest]) {
                dist[edge.dest] = newDist;
                prev[edge.dest] = u;
                pq.push({newDist, edge.dest});
            }
        }
    }
    
    // 回溯路径
    std::vector<int> path;
    if (dist[end] == std::numeric_limits<double>::max()) {
        return PathInfo(path, -1); // 不可达
    }
    
    for (int at = end; at != -1; at = prev[at]) {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());
    
    return PathInfo(path, dist[end]);
}

PathInfo CityGraph::dijkstraAvoid(int start, int end, int avoidCity) const {
    int n = adjacencyList.size();
    std::vector<double> dist(n, std::numeric_limits<double>::max());
    std::vector<int> prev(n, -1);
    std::priority_queue<std::pair<double, int>, 
                       std::vector<std::pair<double, int>>, 
                       std::greater<std::pair<double, int>>> pq;
    
    dist[start] = 0;
    pq.push({0, start});
    
    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();
        
        if (u == end) break;
        if (d > dist[u] || u == avoidCity) continue;
        
        for (const Edge& edge : adjacencyList[u]) {
            if (edge.dest == avoidCity) continue; // 避开指定城市
            
            double newDist = dist[u] + edge.distance;
            if (newDist < dist[edge.dest]) {
                dist[edge.dest] = newDist;
                prev[edge.dest] = u;
                pq.push({newDist, edge.dest});
            }
        }
    }
    
    // 回溯路径
    std::vector<int> path;
    if (dist[end] == std::numeric_limits<double>::max()) {
        return PathInfo(path, -1); // 不可达
    }
    
    for (int at = end; at != -1; at = prev[at]) {
        path.push_back(at);
    }
    std::reverse(path.begin(), path.end());
    
    return PathInfo(path, dist[end]);
}

void CityGraph::computeFloyd() {
    int n = adjacencyList.size();
    floydDistances.assign(n, std::vector<double>(n, std::numeric_limits<double>::max()));
    floydNext.assign(n, std::vector<int>(n, -1));
    
    // 初始化距离矩阵
    for (int i = 0; i < n; i++) {
        floydDistances[i][i] = 0;
        for (const Edge& edge : adjacencyList[i]) {
            floydDistances[i][edge.dest] = edge.distance;
            floydNext[i][edge.dest] = edge.dest;
        }
    }
    
    // Floyd-Warshall算法
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (floydDistances[i][k] != std::numeric_limits<double>::max() &&
                    floydDistances[k][j] != std::numeric_limits<double>::max() &&
                    floydDistances[i][k] + floydDistances[k][j] < floydDistances[i][j]) {
                    floydDistances[i][j] = floydDistances[i][k] + floydDistances[k][j];
                    floydNext[i][j] = floydNext[i][k];
                }
            }
        }
    }
    
    floydComputed = true;
}

PathInfo CityGraph::floyd(int start, int end) {
    if (!floydComputed) {
        computeFloyd();
    }
    
    if (floydDistances[start][end] == std::numeric_limits<double>::max()) {
        return PathInfo(std::vector<int>(), -1); // 不可达
    }
    
    // 重建路径
    std::vector<int> path;
    int current = start;
    path.push_back(current);
    
    while (current != end) {
        current = floydNext[current][end];
        if (current == -1) break;
        path.push_back(current);
    }
    
    return PathInfo(path, floydDistances[start][end]);
}

void CityGraph::findAllPathsDFS(int current, int end, std::vector<int>& currentPath, 
                               double currentDist, int depth, int maxDepth,
                               std::vector<bool>& visited,
                               std::vector<PathInfo>& result) const {
    if (depth > maxDepth) return;
    
    currentPath.push_back(current);
    visited[current] = true;
    
    if (current == end) {
        result.push_back(PathInfo(currentPath, currentDist));
    } else {
        for (const Edge& edge : adjacencyList[current]) {
            if (!visited[edge.dest]) {
                findAllPathsDFS(edge.dest, end, currentPath, 
                               currentDist + edge.distance, 
                               depth + 1, maxDepth,
                               visited, result);
            }
        }
    }
    
    currentPath.pop_back();
    visited[current] = false;
}

std::vector<PathInfo> CityGraph::findAllPaths(int start, int end, int maxDepth) const {
    std::vector<PathInfo> result;
    std::vector<bool> visited(adjacencyList.size(), false);
    std::vector<int> currentPath;
    
    findAllPathsDFS(start, end, currentPath, 0, 0, maxDepth, visited, result);
    
    // 使用快速排序对结果排序
    if (!result.empty()) {
        quickSortPaths(result, 0, result.size() - 1);
    }
    
    return result;
}

void CityGraph::swapPaths(PathInfo& a, PathInfo& b) {
    PathInfo temp = a;
    a = b;
    b = temp;
}

int CityGraph::partition(std::vector<PathInfo>& paths, int low, int high) {
    double pivot = paths[high].distance;
    int i = low - 1;
    
    for (int j = low; j <= high - 1; j++) {
        if (paths[j].distance < pivot) {
            i++;
            swapPaths(paths[i], paths[j]);
        }
    }
    swapPaths(paths[i + 1], paths[high]);
    return i + 1;
}

void CityGraph::quickSortPaths(std::vector<PathInfo>& paths, int low, int high) {
    if (low < high) {
        int pi = partition(paths, low, high);
        quickSortPaths(paths, low, pi - 1);
        quickSortPaths(paths, pi + 1, high);
    }
}

std::vector<PathInfo> CityGraph::kShortestPaths(int start, int end, int k) {
    // 获取第一条最短路径
    PathInfo firstPath = dijkstra(start, end);
    if (firstPath.path.empty()) return {};
    
    std::vector<PathInfo> result;
    result.push_back(firstPath);
    
    // 优先队列存储候选路径
    auto cmp = [](const PathInfo& a, const PathInfo& b) { 
        return a.distance > b.distance; 
    };
    std::priority_queue<PathInfo, std::vector<PathInfo>, decltype(cmp)> candidates(cmp);
    
    for (int i = 1; i < k; i++) {
        const PathInfo& prevPath = result.back();
        
        // 为每个偏离节点生成候选路径
        for (int j = 0; j < prevPath.path.size() - 1; j++) {
            // 复制图
            std::vector<std::vector<Edge>> originalGraph = copyGraph();
            
            int spurNode = prevPath.path[j];
            
            // 移除根路径中的边
            for (const PathInfo& path : result) {
                if (path.path.size() > j && 
                    std::equal(prevPath.path.begin(), 
                              prevPath.path.begin() + j, 
                              path.path.begin())) {
                    int u = path.path[j];
                    int v = path.path[j+1];
                    removeEdge(u, v);
                }
            }
            
            // 计算支路路径
            PathInfo spurPath = dijkstra(spurNode, end);
            
            // 恢复图
            restoreGraph(originalGraph);
            
            if (!spurPath.path.empty()) {
                // 构造完整路径
                std::vector<int> totalPath(prevPath.path.begin(), 
                                         prevPath.path.begin() + j);
                totalPath.insert(totalPath.end(), 
                               spurPath.path.begin(), 
                               spurPath.path.end());
                
                // 计算总距离
                double totalDist = 0;
                for (int m = 0; m < j; m++) {
                    for (const Edge& edge : adjacencyList[prevPath.path[m]]) {
                        if (edge.dest == prevPath.path[m+1]) {
                            totalDist += edge.distance;
                            break;
                        }
                    }
                }
                totalDist += spurPath.distance;
                
                candidates.push(PathInfo(totalPath, totalDist));
            }
        }
        
        if (candidates.empty()) break;
        
        // 获取下一个最短候选路径
        PathInfo nextPath = candidates.top();
        candidates.pop();
        
        // 检查是否已存在
        bool exists = false;
        for (const PathInfo& existing : result) {
            if (existing.path == nextPath.path) {
                exists = true;
                break;
            }
        }
        
        if (!exists) {
            result.push_back(nextPath);
        } else {
            i--; // 重新尝试
        }
    }
    
    return result;
}

std::vector<std::vector<Edge>> CityGraph::copyGraph() const {
    return adjacencyList;
}

void CityGraph::restoreGraph(const std::vector<std::vector<Edge>>& originalGraph) {
    adjacencyList = originalGraph;
}

void CityGraph::removeEdge(int from, int to) {
    auto it = std::find_if(adjacencyList[from].begin(), 
                          adjacencyList[from].end(),
                          [to](const Edge& e) { return e.dest == to; });
    if (it != adjacencyList[from].end()) {
        adjacencyList[from].erase(it);
    }
}

bool CityGraph::hasPath(int start, int end) const {
    std::vector<bool> visited(adjacencyList.size(), false);
    std::queue<int> q;
    q.push(start);
    visited[start] = true;
    
    while (!q.empty()) {
        int current = q.front();
        q.pop();
        
        if (current == end) return true;
        
        for (const Edge& edge : adjacencyList[current]) {
            if (!visited[edge.dest]) {
                visited[edge.dest] = true;
                q.push(edge.dest);
            }
        }
    }
    
    return false;
}

bool CityGraph::validateWuhanCentrality() {
    int wuhanIndex = findCityIndex("武汉");
    if (wuhanIndex == -1) {
        qDebug() << "未找到武汉";
        return false;
    }
    
    int maxHops = 2;
    std::vector<std::vector<int>> hopCount(adjacencyList.size(), 
                                         std::vector<int>(adjacencyList.size(), -1));
    
    // BFS计算所有城市到武汉的最短跳数
    std::queue<std::pair<int, int>> q; // (city, hops)
    hopCount[wuhanIndex][wuhanIndex] = 0;
    q.push({wuhanIndex, 0});
    
    while (!q.empty()) {
        auto [city, hops] = q.front();
        q.pop();
        
        if (hops >= maxHops) continue;
        
        for (const Edge& edge : adjacencyList[city]) {
            if (hopCount[wuhanIndex][edge.dest] == -1) {
                hopCount[wuhanIndex][edge.dest] = hops + 1;
                q.push({edge.dest, hops + 1});
            }
        }
    }
    
    // 检查是否所有城市都在2跳内可达
    int unreachableCount = 0;
    for (int i = 0; i < adjacencyList.size(); i++) {
        if (i != wuhanIndex && hopCount[wuhanIndex][i] == -1) {
            qDebug() << "城市" << cities[i] << "从武汉不可达";
            unreachableCount++;
        } else if (i != wuhanIndex && hopCount[wuhanIndex][i] > maxHops) {
            qDebug() << "城市" << cities[i] << "需要" << hopCount[wuhanIndex][i] << "跳";
            unreachableCount++;
        }
    }
    
    return unreachableCount == 0;
}

void CityGraph::exportResults(const QString& filename, 
                             const std::vector<PathInfo>& allPaths,
                             const PathInfo& dijkstraResult,
                             const PathInfo& floydResult,
                             const QString& startCity,
                             const QString& endCity) const {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法创建输出文件:" << filename;
        return;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    out << "交通咨询系统查询结果\n";
    out << "====================\n\n";
    out << "起点: " << startCity << "\n";
    out << "终点: " << endCity << "\n";
    out << "查询时间: " << QDateTime::currentDateTime().toString() << "\n\n";
    
    // 输出所有路径
    out << "所有可行路径 (" << allPaths.size() << "条):\n";
    out << "------------------------------\n";
    for (int i = 0; i < allPaths.size(); i++) {
        out << "路径" << (i+1) << ": ";
        for (int j = 0; j < allPaths[i].path.size(); j++) {
            out << getCityName(allPaths[i].path[j]);
            if (j < allPaths[i].path.size() - 1) {
                out << " -> ";
            }
        }
        out << "\n总长度: " << QString::number(allPaths[i].distance, 'f', 2) << " km\n\n";
    }
    
    // 输出算法比较结果
    out << "算法比较结果:\n";
    out << "------------------------------\n";
    
    if (!dijkstraResult.path.empty()) {
        out << "[迪杰斯特拉算法] 最短路径:\n";
        for (int i = 0; i < dijkstraResult.path.size(); i++) {
            out << getCityName(dijkstraResult.path[i]);
            if (i < dijkstraResult.path.size() - 1) {
                out << " -> ";
            }
        }
        out << "\n总长度: " << QString::number(dijkstraResult.distance, 'f', 2) << " km\n\n";
    }
    
    if (!floydResult.path.empty()) {
        out << "[费洛伊德算法] 最短路径:\n";
        for (int i = 0; i < floydResult.path.size(); i++) {
            out << getCityName(floydResult.path[i]);
            if (i < floydResult.path.size() - 1) {
                out << " -> ";
            }
        }
        out << "\n总长度: " << QString::number(floydResult.distance, 'f', 2) << " km\n\n";
    }
    
    // 比较结果
    if (!dijkstraResult.path.empty() && !floydResult.path.empty()) {
        out << "算法结果比较:\n";
        if (std::abs(dijkstraResult.distance - floydResult.distance) < 0.01) {
            out << "两种算法得到相同的最短距离: " 
                << QString::number(dijkstraResult.distance, 'f', 2) << " km\n";
        } else {
            out << "算法结果不同:\n";
            out << "Dijkstra: " << QString::number(dijkstraResult.distance, 'f', 2) << " km\n";
            out << "Floyd: " << QString::number(floydResult.distance, 'f', 2) << " km\n";
        }
    }
    
    file.close();
    qDebug() << "结果已导出到:" << filename;
} 