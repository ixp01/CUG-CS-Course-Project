#ifndef CITYGRAPH_H
#define CITYGRAPH_H

#include <vector>
#include <string>
#include <queue>
#include <unordered_map>
#include <limits>
#include <algorithm>
#include <QString>
#include <QStringList>

struct Edge {
    int dest;
    double distance;
    
    Edge(int d, double dist) : dest(d), distance(dist) {}
};

struct PathInfo {
    std::vector<int> path;
    double distance;
    
    PathInfo(const std::vector<int>& p, double d) : path(p), distance(d) {}
};

class CityGraph {
public:
    CityGraph();
    ~CityGraph();
    
    // 数据加载
    bool loadFromCSV(const QString& filename);
    
    // 基本操作
    int findCityIndex(const QString& cityName) const;
    QString getCityName(int index) const;
    int numCities() const;
    const std::vector<QString>& getCityNames() const;
    
    // 最短路径算法
    PathInfo dijkstra(int start, int end) const;
    PathInfo dijkstraAvoid(int start, int end, int avoidCity) const;
    PathInfo floyd(int start, int end);
    
    // 路径遍历算法
    std::vector<PathInfo> findAllPaths(int start, int end, int maxDepth = 10) const;
    
    // K短路径算法 (Yen's algorithm)
    std::vector<PathInfo> kShortestPaths(int start, int end, int k);
    
    // 武汉中心性验证
    bool validateWuhanCentrality();
    
    // 快速排序
    static void quickSortPaths(std::vector<PathInfo>& paths, int low, int high);
    
    // 工具函数
    void exportResults(const QString& filename, 
                      const std::vector<PathInfo>& allPaths,
                      const PathInfo& dijkstraResult,
                      const PathInfo& floydResult,
                      const QString& startCity,
                      const QString& endCity) const;

private:
    std::vector<QString> cities;
    std::vector<std::vector<Edge>> adjacencyList;
    
    // Floyd算法数据
    std::vector<std::vector<double>> floydDistances;
    std::vector<std::vector<int>> floydNext;
    bool floydComputed;
    
    void computeFloyd();
    void findAllPathsDFS(int current, int end, std::vector<int>& currentPath, 
                        double currentDist, int depth, int maxDepth,
                        std::vector<bool>& visited,
                        std::vector<PathInfo>& result) const;
    
    // 快速排序辅助函数
    static int partition(std::vector<PathInfo>& paths, int low, int high);
    static void swapPaths(PathInfo& a, PathInfo& b);
    
    // Yen's算法辅助函数
    std::vector<std::vector<Edge>> copyGraph() const;
    void restoreGraph(const std::vector<std::vector<Edge>>& originalGraph);
    void removeEdge(int from, int to);
    bool hasPath(int start, int end) const;
};

#endif // CITYGRAPH_H 