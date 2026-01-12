# 罗马尼亚度假问题 - 蚁群算法实现

这个项目使用蚁群算法（Ant Colony Optimization, ACO）来解决罗马尼亚度假问题，即在罗马尼亚地图上找到从一个城市到另一个城市的最短路径。

## 功能特点

- 使用蚁群算法寻找最短路径
- 可视化算法收敛过程
- 可视化找到的最优路径
- 支持自定义参数调整

## 安装依赖

```bash
pip install -r requirements.txt
```

## 使用方法

直接运行主程序：

```bash
python romania_aco.py
```

## 参数说明

程序中的主要参数包括：

- `num_ants`: 蚂蚁数量（默认：20）
- `num_iterations`: 迭代次数（默认：100）
- `alpha`: 信息素重要程度因子（默认：1.0）
- `beta`: 启发信息重要程度因子（默认：2.0）
- `rho`: 信息素挥发率（默认：0.5）
- `Q`: 信息素增量常数（默认：100.0）

## 输出说明

程序运行后会输出：
1. 每次迭代的最优路径长度
2. 最终找到的最优路径
3. 最优路径的总长度
4. 算法收敛曲线图
5. 最优路径可视化图

## 示例输出

```
Iteration 1, Best Path Length: 450
Iteration 2, Best Path Length: 420
...
------------------------------------
Starting City: Arad
Ending City: Bucharest
Best Path Found: Arad -> Sibiu -> Rimnicu Vilcea -> Pitesti -> Bucharest
Shortest Distance: 418
------------------------------------
```

## 注意事项

- 程序使用随机数生成，设置了随机种子以确保结果可重现
- 可以通过调整参数来优化算法性能
- 可视化结果会显示在单独的窗口中 