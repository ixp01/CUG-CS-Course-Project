#ifndef GRAPH_H
#define GRAPH_H
#include <QMap>
#include <QList>
#include <QPair>
#include <QString>

/**
 * 罗马尼亚度假问题的数据定义
 * 包含启发函数值和地图邻接表
 */

// 启发式函数值：每个城市到目标城市Bucharest的直线距离
// 这些值用于贪婪算法和A*算法中的启发式搜索
// 启发式函数必须是可采纳的（不高估实际距离），这里使用直线距离保证了这一点
const QMap<QString, int> HEURISTIC = {
    {"Arad", 366},          // Arad到Bucharest的直线距离为366km
    {"Bucharest", 0},       // Bucharest本身，距离为0
    {"Craiova", 160},       // Craiova到Bucharest的直线距离为160km
    {"Drobeta", 242},       // Drobeta到Bucharest的直线距离为242km
    {"Eforie", 161},        // Eforie到Bucharest的直线距离为161km
    {"Fagaras", 176},       // Fagaras到Bucharest的直线距离为176km
    {"Giurgiu", 77},        // Giurgiu到Bucharest的直线距离为77km
    {"Hirsova", 151},       // Hirsova到Bucharest的直线距离为151km
    {"Iasi", 226},          // Iasi到Bucharest的直线距离为226km
    {"Lugoj", 244},         // Lugoj到Bucharest的直线距离为244km
    {"Mehadia", 241},       // Mehadia到Bucharest的直线距离为241km
    {"Neamt", 234},         // Neamt到Bucharest的直线距离为234km
    {"Oradea", 380},        // Oradea到Bucharest的直线距离为380km
    {"Pitesti", 100},       // Pitesti到Bucharest的直线距离为100km
    {"Rimnicu Vilcea", 193}, // Rimnicu Vilcea到Bucharest的直线距离为193km
    {"Sibiu", 253},         // Sibiu到Bucharest的直线距离为253km
    {"Timisoara", 329},     // Timisoara到Bucharest的直线距离为329km
    {"Urziceni", 80},       // Urziceni到Bucharest的直线距离为80km
    {"Vaslui", 199},        // Vaslui到Bucharest的直线距离为199km
    {"Zerind", 374}         // Zerind到Bucharest的直线距离为374km
};

// 罗马尼亚地图的邻接表表示
// 键：城市名称，值：邻居城市列表，每个邻居包含{城市名, 实际距离}
// 这个数据结构定义了所有可能的路径连接和对应的实际行驶距离
const QMap<QString, QList<QPair<QString, int>>> ROMANIA_MAP = {
    {"Arad", {{"Zerind", 75}, {"Sibiu", 140}, {"Timisoara", 118}}},           // Arad的邻居城市及距离
    {"Zerind", {{"Arad", 75}, {"Oradea", 71}}},                              // Zerind的邻居城市及距离
    {"Oradea", {{"Zerind", 71}, {"Sibiu", 151}}},                            // Oradea的邻居城市及距离
    {"Sibiu", {{"Arad", 140}, {"Oradea", 151}, {"Fagaras", 99}, {"Rimnicu Vilcea", 80}}}, // Sibiu的邻居城市及距离
    {"Timisoara", {{"Arad", 118}, {"Lugoj", 111}}},                          // Timisoara的邻居城市及距离
    {"Lugoj", {{"Timisoara", 111}, {"Mehadia", 70}}},                        // Lugoj的邻居城市及距离
    {"Mehadia", {{"Lugoj", 70}, {"Drobeta", 75}}},                           // Mehadia的邻居城市及距离
    {"Drobeta", {{"Mehadia", 75}, {"Craiova", 120}}},                        // Drobeta的邻居城市及距离
    {"Craiova", {{"Drobeta", 120}, {"Rimnicu Vilcea", 146}, {"Pitesti", 138}}}, // Craiova的邻居城市及距离
    {"Rimnicu Vilcea", {{"Sibiu", 80}, {"Craiova", 146}, {"Pitesti", 97}}},  // Rimnicu Vilcea的邻居城市及距离
    {"Fagaras", {{"Sibiu", 99}, {"Bucharest", 211}}},                        // Fagaras的邻居城市及距离
    {"Pitesti", {{"Rimnicu Vilcea", 97}, {"Craiova", 138}, {"Bucharest", 101}}}, // Pitesti的邻居城市及距离
    {"Bucharest", {{"Fagaras", 211}, {"Pitesti", 101}, {"Giurgiu", 90}, {"Urziceni", 85}}}, // Bucharest的邻居城市及距离
    {"Giurgiu", {{"Bucharest", 90}}},                                         // Giurgiu的邻居城市及距离
    {"Urziceni", {{"Bucharest", 85}, {"Hirsova", 98}, {"Vaslui", 142}}},     // Urziceni的邻居城市及距离
    {"Hirsova", {{"Urziceni", 98}, {"Eforie", 86}}},                         // Hirsova的邻居城市及距离
    {"Eforie", {{"Hirsova", 86}}},                                            // Eforie的邻居城市及距离
    {"Vaslui", {{"Urziceni", 142}, {"Iasi", 92}}},                           // Vaslui的邻居城市及距离
    {"Iasi", {{"Vaslui", 92}, {"Neamt", 87}}},                               // Iasi的邻居城市及距离
    {"Neamt", {{"Iasi", 87}}}                                                 // Neamt的邻居城市及距离
};

#endif // GRAPH_H 