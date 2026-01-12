import random
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import networkx as nx
import tkinter as tk
from tkinter import ttk
from tkinter import messagebox

class AntColony:
    def __init__(self, graph, num_ants, num_iterations, alpha, beta, rho, Q, visualize_interval=5):
        self.graph = graph
        self.num_ants = num_ants
        self.num_iterations = num_iterations
        self.alpha = alpha  # 信息素重要程度因子
        self.beta = beta    # 启发信息重要程度因子
        self.rho = rho      # 信息素挥发率
        self.Q = Q          # 信息素增量常数
        self.visualize_interval = visualize_interval
        
        self.pheromones = self._initialize_pheromones()
        self.best_path = None
        self.best_path_length = float('inf')
        self.history = []  # 记录每次迭代的最优路径长度
        self.current_iteration = 0
        self.running = False
        
    def _initialize_pheromones(self):
        """初始化信息素矩阵"""
        pheromones = {}
        for city1 in self.graph:
            pheromones[city1] = {}
            for city2 in self.graph[city1]:
                pheromones[city1][city2] = 1.0
        return pheromones

    def reset(self):
        """重置算法状态"""
        self.pheromones = self._initialize_pheromones()
        self.best_path = None
        self.best_path_length = float('inf')
        self.history = []
        self.current_iteration = 0
        self.running = False

    def _calculate_probability(self, current_city, next_city, visited_cities):
        """计算选择下一城市的概率"""
        if next_city in visited_cities:
            return 0.0

        pheromone = self.pheromones[current_city][next_city]
        distance = self.graph[current_city][next_city]
        heuristic_info = 1.0 / distance if distance != 0 else float('inf')
        
        return (pheromone ** self.alpha) * (heuristic_info ** self.beta)

    def _construct_path(self, start_city, end_city):
        """构建单只蚂蚁的路径"""
        current_city = start_city
        path = [start_city]
        visited_cities = {start_city}
        path_length = 0

        while current_city != end_city:
            possible_next_cities = []
            probabilities = []
            total_probability = 0.0

            for next_city in self.graph[current_city]:
                if next_city not in visited_cities:
                    prob = self._calculate_probability(current_city, next_city, visited_cities)
                    if prob > 0:
                        possible_next_cities.append(next_city)
                        probabilities.append(prob)
                        total_probability += prob

            if not possible_next_cities:
                return None, float('inf')

            # 轮盘赌选择
            normalized_probabilities = [p / total_probability for p in probabilities]
            next_city_selected = random.choices(possible_next_cities, weights=normalized_probabilities, k=1)[0]
            
            path_length += self.graph[current_city][next_city_selected]
            path.append(next_city_selected)
            visited_cities.add(next_city_selected)
            current_city = next_city_selected
            
        return path, path_length

    def _update_pheromones(self, all_ant_paths):
        """更新信息素"""
        # 信息素挥发
        for city1 in self.pheromones:
            for city2 in self.pheromones[city1]:
                self.pheromones[city1][city2] *= (1 - self.rho)

        # 信息素增强
        for path, path_length in all_ant_paths:
            if path_length != float('inf'):
                pheromone_deposit = self.Q / path_length
                for i in range(len(path) - 1):
                    city1, city2 = path[i], path[i+1]
                    self.pheromones[city1][city2] += pheromone_deposit
                    if city2 in self.pheromones and city1 in self.pheromones[city2]:
                        self.pheromones[city2][city1] += pheromone_deposit

    def run_one_iteration(self, start_city, end_city):
        """运行一次迭代"""
        if self.current_iteration >= self.num_iterations:
            self.running = False
            return False
        
        all_ant_paths = []
        for _ in range(self.num_ants):
            path, path_length = self._construct_path(start_city, end_city)
            all_ant_paths.append((path, path_length))

            if path_length < self.best_path_length:
                self.best_path_length = path_length
                self.best_path = path

        self._update_pheromones(all_ant_paths)
        self.history.append(self.best_path_length)
        
        self.current_iteration += 1
        return all_ant_paths


# 罗马尼亚地图数据
romania_map = {
    'Arad': {'Sibiu': 140, 'Zerind': 75, 'Timisoara': 118},
    'Zerind': {'Arad': 75, 'Oradea': 71},
    'Oradea': {'Zerind': 71, 'Sibiu': 151},
    'Sibiu': {'Arad': 140, 'Oradea': 151, 'Fagaras': 99, 'Rimnicu Vilcea': 80},
    'Timisoara': {'Arad': 118, 'Lugoj': 111},
    'Lugoj': {'Timisoara': 111, 'Mehadia': 70},
    'Mehadia': {'Lugoj': 70, 'Drobeta': 75},
    'Drobeta': {'Mehadia': 75, 'Craiova': 120},
    'Craiova': {'Drobeta': 120, 'Rimnicu Vilcea': 146, 'Pitesti': 138},
    'Rimnicu Vilcea': {'Sibiu': 80, 'Craiova': 146, 'Pitesti': 97},
    'Fagaras': {'Sibiu': 99, 'Bucharest': 211},
    'Pitesti': {'Rimnicu Vilcea': 97, 'Craiova': 138, 'Bucharest': 101},
    'Bucharest': {'Fagaras': 211, 'Pitesti': 101, 'Giurgiu': 90, 'Urziceni': 85},
    'Giurgiu': {'Bucharest': 90},
    'Urziceni': {'Bucharest': 85, 'Vaslui': 142, 'Hirsova': 98},
    'Hirsova': {'Urziceni': 98, 'Eforie': 86},
    'Eforie': {'Hirsova': 86},
    'Vaslui': {'Urziceni': 142, 'Iasi': 92},
    'Iasi': {'Vaslui': 92, 'Neamt': 87},
    'Neamt': {'Iasi': 87}
}

class ACOApp:
    def __init__(self, root):
        self.root = root
        self.root.title("蚁群算法 - 罗马尼亚度假问题")
        self.root.geometry("1200x800")
        
        # 创建算法实例
        self.num_ants = tk.IntVar(value=20)
        self.num_iterations = tk.IntVar(value=100)
        self.alpha = tk.DoubleVar(value=1.0)
        self.beta = tk.DoubleVar(value=2.0)
        self.rho = tk.DoubleVar(value=0.5)
        self.Q = tk.DoubleVar(value=100.0)
        self.visualize_interval = tk.IntVar(value=1)
        
        # 创建蚁群算法实例
        self.aco = AntColony(
            romania_map, 
            self.num_ants.get(), 
            self.num_iterations.get(),
            self.alpha.get(),
            self.beta.get(),
            self.rho.get(),
            self.Q.get(),
            self.visualize_interval.get()
        )
        
        # 获取城市列表
        self.cities = sorted(romania_map.keys())
        self.start_city_var = tk.StringVar(value=self.cities[0])  # 默认选择第一个城市
        self.end_city_var = tk.StringVar(value=self.cities[1])    # 默认选择第二个城市
        
        # 创建UI界面
        self.create_ui()
        
    def create_ui(self):
        # 创建主框架
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.pack(fill=tk.BOTH, expand=True)
        
        # 左侧控制区域
        control_frame = ttk.LabelFrame(main_frame, text="控制面板", padding="10")
        control_frame.pack(side=tk.LEFT, fill=tk.Y, padx=5, pady=5)
        
        # 城市选择区域
        city_frame = ttk.LabelFrame(control_frame, text="城市选择", padding="10")
        city_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Label(city_frame, text="起点城市:").grid(row=0, column=0, sticky=tk.W, pady=2)
        start_combo = ttk.Combobox(city_frame, textvariable=self.start_city_var, values=self.cities)
        start_combo.grid(row=0, column=1, sticky=tk.W, pady=2)
        
        ttk.Label(city_frame, text="终点城市:").grid(row=1, column=0, sticky=tk.W, pady=2)
        end_combo = ttk.Combobox(city_frame, textvariable=self.end_city_var, values=self.cities)
        end_combo.grid(row=1, column=1, sticky=tk.W, pady=2)
        
        # 参数设置区域
        param_frame = ttk.LabelFrame(control_frame, text="算法参数", padding="10")
        param_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Label(param_frame, text="蚂蚁数量:").grid(row=0, column=0, sticky=tk.W, pady=2)
        ttk.Spinbox(param_frame, from_=1, to=100, textvariable=self.num_ants, width=10).grid(row=0, column=1, sticky=tk.W, pady=2)
        
        ttk.Label(param_frame, text="迭代次数:").grid(row=1, column=0, sticky=tk.W, pady=2)
        ttk.Spinbox(param_frame, from_=1, to=1000, textvariable=self.num_iterations, width=10).grid(row=1, column=1, sticky=tk.W, pady=2)
        
        ttk.Label(param_frame, text="α (信息素重要度):").grid(row=2, column=0, sticky=tk.W, pady=2)
        ttk.Spinbox(param_frame, from_=0.1, to=10.0, increment=0.1, textvariable=self.alpha, width=10).grid(row=2, column=1, sticky=tk.W, pady=2)
        
        ttk.Label(param_frame, text="β (启发信息重要度):").grid(row=3, column=0, sticky=tk.W, pady=2)
        ttk.Spinbox(param_frame, from_=0.1, to=10.0, increment=0.1, textvariable=self.beta, width=10).grid(row=3, column=1, sticky=tk.W, pady=2)
        
        ttk.Label(param_frame, text="ρ (信息素挥发率):").grid(row=4, column=0, sticky=tk.W, pady=2)
        ttk.Spinbox(param_frame, from_=0.1, to=1.0, increment=0.1, textvariable=self.rho, width=10).grid(row=4, column=1, sticky=tk.W, pady=2)
        
        ttk.Label(param_frame, text="Q (信息素增量常数):").grid(row=5, column=0, sticky=tk.W, pady=2)
        ttk.Spinbox(param_frame, from_=1.0, to=1000.0, increment=10.0, textvariable=self.Q, width=10).grid(row=5, column=1, sticky=tk.W, pady=2)
        
        ttk.Label(param_frame, text="可视化间隔:").grid(row=6, column=0, sticky=tk.W, pady=2)
        ttk.Spinbox(param_frame, from_=1, to=10, textvariable=self.visualize_interval, width=10).grid(row=6, column=1, sticky=tk.W, pady=2)
        
        # 控制按钮区域
        button_frame = ttk.Frame(control_frame, padding="10")
        button_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.start_button = ttk.Button(button_frame, text="开始", command=self.start_algorithm)
        self.start_button.pack(side=tk.LEFT, padx=5)
        
        self.pause_button = ttk.Button(button_frame, text="暂停", command=self.pause_algorithm, state=tk.DISABLED)
        self.pause_button.pack(side=tk.LEFT, padx=5)
        
        self.step_button = ttk.Button(button_frame, text="单步", command=self.step_algorithm)
        self.step_button.pack(side=tk.LEFT, padx=5)
        
        self.reset_button = ttk.Button(button_frame, text="重置", command=self.reset_algorithm)
        self.reset_button.pack(side=tk.LEFT, padx=5)
        
        # 信息显示区域
        info_frame = ttk.LabelFrame(control_frame, text="运行信息", padding="10")
        info_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.info_text = tk.Text(info_frame, height=10, width=30)
        self.info_text.pack(fill=tk.BOTH, expand=True)
        
        # 右侧可视化区域
        vis_frame = ttk.LabelFrame(main_frame, text="可视化", padding="10")
        vis_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # 创建两个图形区域
        fig_frame = ttk.Frame(vis_frame)
        fig_frame.pack(fill=tk.BOTH, expand=True)
        
        # 路径图
        self.fig1 = plt.Figure(figsize=(6, 5))
        self.ax1 = self.fig1.add_subplot(111)
        self.canvas1 = FigureCanvasTkAgg(self.fig1, master=fig_frame)
        self.canvas1.get_tk_widget().pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        # 收敛曲线图
        self.fig2 = plt.Figure(figsize=(6, 5))
        self.ax2 = self.fig2.add_subplot(111)
        self.canvas2 = FigureCanvasTkAgg(self.fig2, master=fig_frame)
        self.canvas2.get_tk_widget().pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)
        
        # 初始化图形
        self.initialize_plots()
        
    def initialize_plots(self):
        # 初始化路径图
        self.ax1.clear()
        G = nx.Graph()
        
        # 添加所有边
        for city1 in romania_map:
            for city2, distance in romania_map[city1].items():
                G.add_edge(city1, city2, weight=distance)
        
        # 使用spring布局，保持布局一致
        self.pos = nx.spring_layout(G, seed=42)
        
        # 绘制所有边作为背景
        nx.draw_networkx_edges(G, self.pos, edge_color='lightgray', alpha=0.3, ax=self.ax1)
        
        # 绘制节点
        nx.draw_networkx_nodes(G, self.pos, node_color='lightblue', node_size=500, ax=self.ax1)
        nx.draw_networkx_labels(G, self.pos, ax=self.ax1)
        
        self.ax1.set_title('罗马尼亚地图')
        self.ax1.axis('off')
        
        # 初始化收敛曲线图
        self.ax2.clear()
        self.ax2.set_title('算法收敛曲线')
        self.ax2.set_xlabel('迭代次数')
        self.ax2.set_ylabel('最优路径长度')
        self.ax2.grid(True)
        
        # 更新画布
        self.canvas1.draw()
        self.canvas2.draw()
    
    def update_algorithm_params(self):
        """更新算法参数"""
        self.aco.num_ants = self.num_ants.get()
        self.aco.num_iterations = self.num_iterations.get()
        self.aco.alpha = self.alpha.get()
        self.aco.beta = self.beta.get()
        self.aco.rho = self.rho.get()
        self.aco.Q = self.Q.get()
        self.aco.visualize_interval = self.visualize_interval.get()
    
    def start_algorithm(self):
        """开始算法"""
        if self.start_city_var.get() == self.end_city_var.get():
            messagebox.showerror("错误", "起点和终点不能相同！")
            return
        
        # 更新参数
        self.update_algorithm_params()
        
        # 更新按钮状态
        self.start_button.config(state=tk.DISABLED)
        self.pause_button.config(state=tk.NORMAL)
        self.step_button.config(state=tk.DISABLED)
        
        # 开始运行
        self.aco.running = True
        self.log_info(f"开始寻找从 {self.start_city_var.get()} 到 {self.end_city_var.get()} 的路径...")
        self.run_iteration()
    
    def pause_algorithm(self):
        """暂停算法"""
        self.aco.running = False
        self.start_button.config(state=tk.NORMAL)
        self.pause_button.config(state=tk.DISABLED)
        self.step_button.config(state=tk.NORMAL)
        self.log_info("算法已暂停")
    
    def step_algorithm(self):
        """单步执行算法"""
        if self.start_city_var.get() == self.end_city_var.get():
            messagebox.showerror("错误", "起点和终点不能相同！")
            return
            
        # 更新参数
        self.update_algorithm_params()
        
        # 执行一步
        self.execute_one_step()
        
    def reset_algorithm(self):
        """重置算法"""
        self.aco.reset()
        self.initialize_plots()
        self.start_button.config(state=tk.NORMAL)
        self.pause_button.config(state=tk.DISABLED)
        self.step_button.config(state=tk.NORMAL)
        self.info_text.delete(1.0, tk.END)
        self.log_info("算法已重置")
    
    def run_iteration(self):
        """运行算法迭代"""
        if not self.aco.running:
            return
            
        # 执行一步
        finished = self.execute_one_step()
        
        if not finished and self.aco.running:
            # 如果未完成且仍在运行，则继续下一步
            self.root.after(100, self.run_iteration)
        else:
            # 完成后更新按钮状态
            self.start_button.config(state=tk.NORMAL)
            self.pause_button.config(state=tk.DISABLED)
            self.step_button.config(state=tk.NORMAL)
            if finished:
                self.log_info(f"算法已完成! 共运行 {self.aco.current_iteration} 次迭代")
                self.log_info(f"最优路径: {' -> '.join(self.aco.best_path)}")
                self.log_info(f"最短距离: {self.aco.best_path_length}")
    
    def execute_one_step(self):
        """执行算法的一步"""
        start_city = self.start_city_var.get()
        end_city = self.end_city_var.get()
        
        # 运行一次迭代
        all_ant_paths = self.aco.run_one_iteration(start_city, end_city)
        
        if all_ant_paths is False:  # 算法已完成
            return True
            
        # 更新信息
        iteration = self.aco.current_iteration
        self.log_info(f"迭代 {iteration}, 最优路径长度: {self.aco.best_path_length}")
        
        # 更新可视化
        if iteration % self.aco.visualize_interval == 0 or iteration == 1:
            self.update_visualization(all_ant_paths, start_city, end_city)
            
        return False
    
    def update_visualization(self, all_ant_paths, start_city, end_city):
        """更新可视化"""
        # 更新路径图
        self.ax1.clear()
        G = nx.Graph()
        
        # 添加所有边
        for city1 in romania_map:
            for city2, distance in romania_map[city1].items():
                G.add_edge(city1, city2, weight=distance)
        
        # 绘制所有边作为背景
        nx.draw_networkx_edges(G, self.pos, edge_color='lightgray', alpha=0.3, ax=self.ax1)
        
        # 绘制当前迭代中每只蚂蚁的路径
        for path, path_length in all_ant_paths:
            if path is not None and path_length != float('inf'):
                path_edges = list(zip(path[:-1], path[1:]))
                nx.draw_networkx_edges(G, self.pos, edgelist=path_edges, 
                                     edge_color='blue', alpha=0.1, 
                                     style='dashed', ax=self.ax1)
        
        # 绘制当前最优路径
        if self.aco.best_path:
            best_path_edges = list(zip(self.aco.best_path[:-1], self.aco.best_path[1:]))
            nx.draw_networkx_edges(G, self.pos, edgelist=best_path_edges, 
                                 edge_color='red', width=3, 
                                 style='solid', ax=self.ax1)
        
        # 绘制信息素强度
        max_pheromone = max(max(inner.values()) for inner in self.aco.pheromones.values())
        for u, v in G.edges():
            if u in self.aco.pheromones and v in self.aco.pheromones[u]:
                pheromone_value = self.aco.pheromones[u][v]
            elif v in self.aco.pheromones and u in self.aco.pheromones[v]:
                pheromone_value = self.aco.pheromones[v][u]
            else:
                pheromone_value = 0
                
            if max_pheromone > 0:
                edge_width = 0.5 + 3 * (pheromone_value / max_pheromone)
            else:
                edge_width = 0.5
                
            nx.draw_networkx_edges(G, self.pos, edgelist=[(u,v)], 
                                 width=edge_width, edge_color='green', 
                                 alpha=0.6, ax=self.ax1)
        
        # 绘制节点
        nx.draw_networkx_nodes(G, self.pos, node_color='lightblue', node_size=500, ax=self.ax1)
        
        # 特别标记起点和终点
        nx.draw_networkx_nodes(G, self.pos, nodelist=[start_city], 
                              node_color='green', node_size=700, ax=self.ax1)
        nx.draw_networkx_nodes(G, self.pos, nodelist=[end_city], 
                              node_color='red', node_size=700, ax=self.ax1)
        
        nx.draw_networkx_labels(G, self.pos, ax=self.ax1)
        
        self.ax1.set_title(f'迭代 {self.aco.current_iteration}\n最优路径长度: {self.aco.best_path_length:.2f}')
        self.ax1.axis('off')
        
        # 更新收敛曲线图
        self.ax2.clear()
        self.ax2.plot(self.aco.history)
        self.ax2.set_title('算法收敛曲线')
        self.ax2.set_xlabel('迭代次数')
        self.ax2.set_ylabel('最优路径长度')
        self.ax2.grid(True)
        
        # 更新画布
        self.canvas1.draw()
        self.canvas2.draw()
    
    def log_info(self, message):
        """记录信息到日志区域"""
        self.info_text.insert(tk.END, message + "\n")
        self.info_text.see(tk.END)

if __name__ == "__main__":
    # 设置随机种子以确保结果可重现
    random.seed(42)
    
    # 创建GUI应用
    root = tk.Tk()
    app = ACOApp(root)
    root.mainloop()
