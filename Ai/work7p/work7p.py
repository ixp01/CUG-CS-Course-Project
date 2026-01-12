import tkinter as tk
from tkinter import ttk, messagebox
import numpy as np
import random
import collections
import time
import threading
import os

class WumpusWorldEnv:
    """Wumpus World环境类"""
    def __init__(self, size=4, num_pits=3, has_wumpus=True, random_seed=None):
        if random_seed is not None:
            random.seed(random_seed)
            np.random.seed(random_seed)
            
        self.size = size
        self.grid = [[' ' for _ in range(size)] for _ in range(size)]
        self.start_pos = (0, 0)
        self.current_pos = self.start_pos
        self.current_dir = 'North'
        self.has_arrow = True
        self.has_gold = False
        self.score = 0
        self.game_over = False
        self.wumpus_alive = True
        self.scream_percept = False
        self.bump_percept = False
        
        # 设置起点
        self.grid[0][0] = 'S'
        
        # 放置元素
        self._place_elements(num_pits, has_wumpus)
        
    def _place_elements(self, num_pits, has_wumpus):
        """放置Wumpus、坑洞和金子"""
        available_positions = []
        for r in range(self.size):
            for c in range(self.size):
                if (r, c) != self.start_pos:  # 起点不能放置元素
                    available_positions.append((r, c))
        
        # 放置Wumpus
        if has_wumpus:
            self.wumpus_pos = random.choice(available_positions)
            self.grid[self.wumpus_pos[0]][self.wumpus_pos[1]] = 'W'
            available_positions.remove(self.wumpus_pos)
        else:
            self.wumpus_pos = None
        
        # 放置坑洞
        self.pit_positions = []
        for _ in range(min(num_pits, len(available_positions))):
            if available_positions:
                pit_pos = random.choice(available_positions)
                self.pit_positions.append(pit_pos)
                self.grid[pit_pos[0]][pit_pos[1]] = 'P'
                available_positions.remove(pit_pos)
        
        # 放置金子
        if available_positions:
            self.gold_pos = random.choice(available_positions)
            self.grid[self.gold_pos[0]][self.gold_pos[1]] = 'G'
        else:
            self.gold_pos = None
    
    def reset(self):
        """重置环境"""
        self.current_pos = self.start_pos
        self.current_dir = 'North'
        self.has_arrow = True
        self.has_gold = False
        self.score = 0
        self.game_over = False
        self.wumpus_alive = True
        self.scream_percept = False
        self.bump_percept = False
        
        # 恢复金子（如果被抓走了）
        if self.gold_pos and self.grid[self.gold_pos[0]][self.gold_pos[1]] != 'G':
            self.grid[self.gold_pos[0]][self.gold_pos[1]] = 'G'
        
        return self._get_current_state()
    
    def _get_current_state(self):
        """获取当前状态"""
        # 状态包括：位置(r,c)、方向、是否有箭、是否有金子
        dir_map = {'North': 0, 'East': 1, 'South': 2, 'West': 3}
        return (self.current_pos[0], self.current_pos[1], 
                dir_map[self.current_dir], self.has_arrow, self.has_gold)
    
    def _get_percepts_at_pos(self, r, c):
        """获取指定位置的感知"""
        percepts = []
        
        # 检查相邻位置的Wumpus（Stench）
        for dr, dc in [(-1,0), (1,0), (0,-1), (0,1)]:
            nr, nc = r + dr, c + dc
            if (0 <= nr < self.size and 0 <= nc < self.size and 
                self.grid[nr][nc] == 'W' and self.wumpus_alive):
                percepts.append('Stench')
                break
        
        # 检查相邻位置的坑洞（Breeze）
        for dr, dc in [(-1,0), (1,0), (0,-1), (0,1)]:
            nr, nc = r + dr, c + dc
            if (0 <= nr < self.size and 0 <= nc < self.size and 
                self.grid[nr][nc] == 'P'):
                percepts.append('Breeze')
                break
        
        # 检查当前位置的金子（Glitter）
        if self.grid[r][c] == 'G':
            percepts.append('Glitter')
        
        # 其他感知
        if self.scream_percept:
            percepts.append('Scream')
        if self.bump_percept:
            percepts.append('Bump')
        
        return percepts
    
    def step(self, action):
        """执行动作 - 修复版本"""
        reward = -1  # 基础移动惩罚
        done = False
        info = {}
        
        self.scream_percept = False
        self.bump_percept = False
        
        r, c = self.current_pos
        
        if action == 'Forward':
            if self.current_dir == 'North':
                next_r, next_c = r + 1, c
            elif self.current_dir == 'South':
                next_r, next_c = r - 1, c
            elif self.current_dir == 'East':
                next_r, next_c = r, c + 1
            elif self.current_dir == 'West':
                next_r, next_c = r, c - 1
                
            if 0 <= next_r < self.size and 0 <= next_c < self.size:
                self.current_pos = (next_r, next_c)
                
                if (self.current_pos == self.wumpus_pos and self.wumpus_alive):
                    reward = -1000
                    done = True
                    info['death_reason'] = 'Wumpus'
                elif self.grid[next_r][next_c] == 'P':
                    reward = -1000
                    done = True
                    info['death_reason'] = 'Pit'
            else:
                self.bump_percept = True
                reward = -10
                info['bump'] = True
                
        elif action == 'TurnLeft':
            dirs = ['North', 'West', 'South', 'East']
            curr_idx = dirs.index(self.current_dir)
            self.current_dir = dirs[(curr_idx + 1) % 4]
            
        elif action == 'TurnRight':
            dirs = ['North', 'East', 'South', 'West']
            curr_idx = dirs.index(self.current_dir)
            self.current_dir = dirs[(curr_idx + 1) % 4]
            
        elif action == 'Shoot':
            reward = -10  # 射击消耗箭矢
            if self.has_arrow:
                self.has_arrow = False
                if self._is_wumpus_in_sight():
                    self.wumpus_alive = False
                    self.scream_percept = True
                    reward += 50
                    info['killed_wumpus'] = True
            else:
                reward = -20  # 没有箭矢却射击
                
        elif action == 'Grab':
            # 修复Grab逻辑：只有在金子位置且没有金子时才能抓取
            if self.current_pos == self.gold_pos and not self.has_gold:
                self.has_gold = True
                self.grid[r][c] = ' '  # 移除金子标记
                reward += 100
                info['grabbed_gold'] = True
            else:
                # 无效的Grab操作应该有惩罚
                reward = -5
                info['invalid_grab'] = True
                
        elif action == 'Climb':
            if self.current_pos == self.start_pos:
                if self.has_gold:
                    reward += 1000
                    done = True
                    info['success'] = True
                else:
                    reward = -10  # 没有金子就爬出去
            else:
                reward = -100  # 不在起点却尝试爬出
                
        next_state = self._get_current_state()
        
        if done:
            self.game_over = True
            
        self.score += reward
        return next_state, reward, done, info

    def _is_wumpus_in_sight(self):
        """检查Wumpus是否在视线范围内"""
        if not self.wumpus_alive or self.wumpus_pos is None:
            return False
            
        r, c = self.current_pos
        wr, wc = self.wumpus_pos
        
        if self.current_dir == 'North' and c == wc and r < wr:
            return True
        elif self.current_dir == 'South' and c == wc and r > wr:
            return True
        elif self.current_dir == 'East' and r == wr and c < wc:
            return True
        elif self.current_dir == 'West' and r == wr and c > wc:
            return True
            
        return False

def optimized_train_agent(env, num_episodes=1000, max_steps=200, learning_rate=0.1, 
                         discount_factor=0.95, epsilon_start=0.9, epsilon_min=0.05, 
                         epsilon_decay_rate=0.002, display_interval=100):
    """GUI专用的优化训练算法"""
    
    # 初始化Q表，给不同动作设置更智能的初始值
    def init_q_values():
        return {'Forward': 0.3, 'TurnLeft': 0.0, 'TurnRight': 0.0, 
                'Shoot': -0.3, 'Grab': 0.0, 'Climb': 1.0}
    
    q_table = collections.defaultdict(init_q_values)
    
    episode_rewards = []
    success_count = 0
    exploration_bonus = {}  # 记录位置探索次数
    
    print("开始Q-learning训练（GUI优化版本）...")
    
    for episode in range(num_episodes):
        state = env.reset()
        total_reward = 0
        done = False
        steps = 0
        visited_positions = set()
        visited_positions.add(state[:2])
        
        # 改进的epsilon衰减策略
        progress = episode / num_episodes
        epsilon = epsilon_min + (epsilon_start - epsilon_min) * np.exp(-epsilon_decay_rate * episode)
        
        while not done and steps < max_steps:
            # 超智能探索策略
            if random.uniform(0, 1) < epsilon:
                current_pos = state[:2]
                has_gold = state[4]
                has_arrow = state[3]
                percepts = env._get_percepts_at_pos(*current_pos)
                
                # 基于感知的智能决策 - 修复逻辑错误
                if 'Glitter' in percepts and not has_gold:
                    action = 'Grab'  # 优先级1：看到金子且没有金子时立即抓取
                elif has_gold and current_pos == env.start_pos:
                    action = 'Climb'  # 优先级2：有金子在起点立即逃脱
                elif has_gold and current_pos != env.start_pos:
                    # 有金子但不在起点，优先返回起点
                    if random.uniform(0, 1) < 0.8:
                        action = 'Forward'  # 80%概率前进
                    else:
                        action = random.choice(['TurnLeft', 'TurnRight'])
                elif 'Stench' in percepts and has_arrow and random.uniform(0, 1) < 0.3:
                    action = 'Shoot'  # 30%概率射击Wumpus
                elif 'Breeze' in percepts and random.uniform(0, 1) < 0.7:
                    action = random.choice(['TurnLeft', 'TurnRight'])  # 感知到坑洞时转向
                elif steps < 5 or random.uniform(0, 1) < 0.6:
                    action = 'Forward'  # 前期或60%概率前进探索
                else:
                    # 智能转向：避免长时间同一方向
                    action = random.choice(['Forward', 'TurnLeft', 'TurnRight'])
            else:
                # 利用阶段：选择最佳动作，但要验证动作的合理性
                current_pos = state[:2]
                has_gold = state[4]
                percepts = env._get_percepts_at_pos(*current_pos)
                
                # 获取Q值最高的动作
                best_action = max(q_table[state], key=q_table[state].get)
                
                # 验证动作合理性
                if best_action == 'Grab':
                    if 'Glitter' in percepts and not has_gold:
                        action = 'Grab'  # 合理的抓取
                    else:
                        # 不合理的抓取，选择第二好的动作
                        q_values = q_table[state].copy()
                        del q_values['Grab']
                        action = max(q_values, key=q_values.get) if q_values else 'Forward'
                elif best_action == 'Climb':
                    if current_pos == env.start_pos:
                        action = 'Climb'  # 合理的攀爬
                    else:
                        # 不在起点，选择其他动作
                        action = 'Forward' if random.uniform(0, 1) < 0.7 else random.choice(['TurnLeft', 'TurnRight'])
                else:
                    action = best_action
                
            next_state, reward, done, info = env.step(action)
            total_reward += reward
            
            # 高级奖励塑形系统
            shaped_reward = reward
            current_pos = next_state[:2]
            has_gold = next_state[4]
            
            # 探索奖励系统
            if action == 'Forward' and reward == -1:
                if current_pos not in exploration_bonus:
                    exploration_bonus[current_pos] = 0
                    shaped_reward = -0.2  # 首次访问大幅减少惩罚
                    visited_positions.add(current_pos)
                elif exploration_bonus[current_pos] < 2:
                    shaped_reward = -0.5  # 少量重复访问轻微惩罚
                else:
                    shaped_reward = -1.2  # 过度重复访问重惩罚
                exploration_bonus[current_pos] += 1
            
            # 行为奖励塑形
            elif action in ['TurnLeft', 'TurnRight']:
                if steps > 10:
                    shaped_reward = reward - 1.5  # 后期转向重惩罚
                elif steps > 5:
                    shaped_reward = reward - 0.5
            elif action == 'Grab':
                if 'grabbed_gold' in info:
                    shaped_reward = reward + 80  # 大幅奖励抓金子
                elif 'invalid_grab' in info:
                    shaped_reward = reward - 10  # 惩罚无效抓取
            elif action == 'Climb' and 'success' in info:
                shaped_reward = reward + 150  # 大幅奖励成功逃脱
                success_count += 1
            elif action == 'Shoot':
                if 'killed_wumpus' in info:
                    shaped_reward = reward + 30  # 成功射击奖励
                else:
                    shaped_reward = reward - 5   # 浪费箭矢惩罚
            
            # 自适应学习率策略
            if shaped_reward > 50:  # 重要正面经验
                adaptive_lr = learning_rate * 2.0
            elif shaped_reward > 0:   # 一般正面经验
                adaptive_lr = learning_rate * 1.3
            elif shaped_reward < -500:  # 致命错误
                adaptive_lr = learning_rate * 1.5
            else:
                adaptive_lr = learning_rate
            
            # Q值更新
            old_q_value = q_table[state][action]
            next_max_q = max(q_table[next_state].values()) if q_table[next_state] else 0
            
            new_q_value = old_q_value + adaptive_lr * (
                shaped_reward + discount_factor * next_max_q - old_q_value)
            
            q_table[state][action] = new_q_value
            
            state = next_state
            steps += 1
            
        episode_rewards.append(total_reward)
        
        # 改进的进度显示
        if (episode + 1) % display_interval == 0 or episode == 0:
            recent_rewards = episode_rewards[-display_interval:]
            avg_reward = sum(recent_rewards) / len(recent_rewards)
            success_rate = success_count / (episode + 1) * 100
            exploration_coverage = len(exploration_bonus) / 16 * 100  # 4x4网格
            
            print(f"回合 {episode+1}/{num_episodes}, "
                  f"平均奖励: {avg_reward:.2f}, "
                  f"成功率: {success_rate:.1f}%, "
                  f"探索覆盖: {exploration_coverage:.1f}%, "
                  f"Epsilon: {epsilon:.4f}")
    
    print(f"训练完成! 总成功次数: {success_count}/{num_episodes}, "
          f"最终探索覆盖: {len(exploration_bonus)}/16格")
    return q_table, episode_rewards

class WumpusGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Wumpus世界 - 强化学习可视化")
        self.root.geometry("1200x800")
        
        # 初始化环境
        self.use_random_map = True  # 默认使用随机地图
        self.env = WumpusWorldEnv(size=4, num_pits=2, has_wumpus=True, random_seed=None if self.use_random_map else 42)
        self.q_table = None
        self.is_training = False
        self.is_demo_running = False
        self.show_hidden = False  # 是否显示隐藏元素
        self.game_completed = False  # 游戏是否成功完成
        self.visited_cells = set()  # 记录访问过的格子
        self.visited_cells.add(self.env.start_pos)  # 起始位置已知
        
        # 创建GUI组件
        self.create_widgets()
        self.update_display()
        
    def create_widgets(self):
        """创建GUI组件"""
        # 主框架
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # 左侧：游戏区域
        game_frame = ttk.LabelFrame(main_frame, text="游戏区域", padding="10")
        game_frame.grid(row=0, column=0, padx=(0, 10), sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # 创建网格画布
        self.canvas = tk.Canvas(game_frame, width=400, height=400, bg="white")
        self.canvas.grid(row=0, column=0, columnspan=3)
        
        # 状态信息
        self.status_frame = ttk.LabelFrame(game_frame, text="状态信息", padding="5")
        self.status_frame.grid(row=1, column=0, columnspan=3, pady=(10, 0), sticky=(tk.W, tk.E))
        
        self.status_labels = {}
        status_items = ["位置", "方向", "得分", "有箭", "有金子", "感知"]
        for i, item in enumerate(status_items):
            ttk.Label(self.status_frame, text=f"{item}:").grid(row=i//2, column=(i%2)*2, sticky=tk.W)
            self.status_labels[item] = ttk.Label(self.status_frame, text="", foreground="blue")
            self.status_labels[item].grid(row=i//2, column=(i%2)*2+1, sticky=tk.W, padx=(5, 20))
        
        # 右侧：控制面板
        control_frame = ttk.LabelFrame(main_frame, text="控制面板", padding="10")
        control_frame.grid(row=0, column=1, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # 手动控制按钮
        manual_frame = ttk.LabelFrame(control_frame, text="手动控制", padding="5")
        manual_frame.grid(row=0, column=0, sticky=(tk.W, tk.E), pady=(0, 10))
        
        ttk.Button(manual_frame, text="前进", command=self.manual_forward).grid(row=0, column=1, padx=5, pady=2)
        ttk.Button(manual_frame, text="左转", command=self.manual_turn_left).grid(row=1, column=0, padx=5, pady=2)
        ttk.Button(manual_frame, text="右转", command=self.manual_turn_right).grid(row=1, column=2, padx=5, pady=2)
        ttk.Button(manual_frame, text="射击", command=self.manual_shoot).grid(row=2, column=0, padx=5, pady=2)
        ttk.Button(manual_frame, text="抓取", command=self.manual_grab).grid(row=2, column=1, padx=5, pady=2)
        ttk.Button(manual_frame, text="攀爬", command=self.manual_climb).grid(row=2, column=2, padx=5, pady=2)
        
        # 训练控制
        train_frame = ttk.LabelFrame(control_frame, text="AI训练", padding="5")
        train_frame.grid(row=1, column=0, sticky=(tk.W, tk.E), pady=(0, 10))
        
        # 训练参数
        ttk.Label(train_frame, text="训练回合数:").grid(row=0, column=0, sticky=tk.W)
        self.episodes_var = tk.StringVar(value="3000")
        ttk.Entry(train_frame, textvariable=self.episodes_var, width=10).grid(row=0, column=1, padx=5)
        
        ttk.Label(train_frame, text="学习率:").grid(row=1, column=0, sticky=tk.W)
        self.lr_var = tk.StringVar(value="0.15")
        ttk.Entry(train_frame, textvariable=self.lr_var, width=10).grid(row=1, column=1, padx=5)
        
        # 训练按钮
        self.train_button = ttk.Button(train_frame, text="开始训练", command=self.start_training)
        self.train_button.grid(row=2, column=0, columnspan=2, pady=10)
        
        # 训练进度
        self.progress_var = tk.StringVar(value="未开始训练")
        ttk.Label(train_frame, textvariable=self.progress_var).grid(row=3, column=0, columnspan=2)
        
        # AI演示
        demo_frame = ttk.LabelFrame(control_frame, text="AI演示", padding="5")
        demo_frame.grid(row=2, column=0, sticky=(tk.W, tk.E), pady=(0, 10))
        
        self.demo_button = ttk.Button(demo_frame, text="开始AI演示", command=self.start_demo, state="disabled")
        self.demo_button.grid(row=0, column=0, pady=5)
        
        self.stop_demo_button = ttk.Button(demo_frame, text="停止演示", command=self.stop_demo, state="disabled")
        self.stop_demo_button.grid(row=0, column=1, padx=(10, 0), pady=5)
        
        # 演示速度控制
        ttk.Label(demo_frame, text="演示速度:").grid(row=1, column=0, sticky=tk.W)
        self.speed_var = tk.DoubleVar(value=1.0)
        speed_scale = ttk.Scale(demo_frame, from_=0.1, to=3.0, orient=tk.HORIZONTAL, variable=self.speed_var)
        speed_scale.grid(row=1, column=1, sticky=(tk.W, tk.E))
        
        # 重置按钮
        ttk.Button(control_frame, text="重置游戏", command=self.reset_game).grid(row=3, column=0, pady=10)
        
        # 显示选项
        display_frame = ttk.LabelFrame(control_frame, text="游戏选项", padding="5")
        display_frame.grid(row=4, column=0, sticky=(tk.W, tk.E), pady=(10, 0))
        
        self.show_hidden_var = tk.BooleanVar()
        ttk.Checkbutton(display_frame, text="显示隐藏元素（作弊模式）", variable=self.show_hidden_var, 
                       command=self.toggle_hidden).grid(row=0, column=0, sticky=tk.W)
        
        self.random_map_var = tk.BooleanVar(value=True)
        ttk.Checkbutton(display_frame, text="使用随机地图", variable=self.random_map_var, 
                       command=self.toggle_random_map).grid(row=1, column=0, sticky=tk.W)
        
        ttk.Button(display_frame, text="生成新地图", command=self.generate_new_map).grid(row=2, column=0, pady=5)
        
        # 底部：日志区域
        log_frame = ttk.LabelFrame(main_frame, text="游戏日志", padding="5")
        log_frame.grid(row=1, column=0, columnspan=2, sticky=(tk.W, tk.E, tk.N, tk.S), pady=(10, 0))
        
        # 创建文本框和滚动条
        self.log_text = tk.Text(log_frame, height=8, width=80)
        scrollbar = ttk.Scrollbar(log_frame, orient=tk.VERTICAL, command=self.log_text.yview)
        self.log_text.configure(yscrollcommand=scrollbar.set)
        
        self.log_text.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        scrollbar.grid(row=0, column=1, sticky=(tk.N, tk.S))
        
        # 配置网格权重
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)
        main_frame.columnconfigure(0, weight=2)
        main_frame.columnconfigure(1, weight=1)
        main_frame.rowconfigure(1, weight=1)
        log_frame.columnconfigure(0, weight=1)
        log_frame.rowconfigure(0, weight=1)
        
    def update_display(self):
        """更新显示"""
        self.draw_world()
        self.update_status()
        
    def draw_world(self):
        """绘制游戏世界"""
        self.canvas.delete("all")
        
        # 计算网格大小
        grid_size = 400 // self.env.size
        
        # 绘制网格
        for i in range(self.env.size + 1):
            # 垂直线
            x = i * grid_size
            self.canvas.create_line(x, 0, x, 400, fill="black", width=2)
            # 水平线
            y = i * grid_size
            self.canvas.create_line(0, y, 400, y, fill="black", width=2)
        
        # 绘制元素
        for r in range(self.env.size):
            for c in range(self.env.size):
                # 计算屏幕坐标（注意y轴翻转）
                x1 = c * grid_size
                y1 = (self.env.size - 1 - r) * grid_size
                x2 = x1 + grid_size
                y2 = y1 + grid_size
                center_x = (x1 + x2) // 2
                center_y = (y1 + y2) // 2
                
                # 背景色
                if (r, c) == self.env.start_pos:
                    self.canvas.create_rectangle(x1, y1, x2, y2, fill="lightgreen", outline="black")
                else:
                    self.canvas.create_rectangle(x1, y1, x2, y2, fill="white", outline="black")
                
                # 绘制元素（只在特定条件下显示）
                cell_visited = (r, c) in self.visited_cells
                show_element = self.show_hidden or (self.game_completed and cell_visited)
                
                if self.env.grid[r][c] == 'P':
                    # 坑洞：只有游戏成功完成后或开启作弊模式时才显示
                    if show_element or (cell_visited and (r, c) == self.env.current_pos):
                        self.canvas.create_oval(x1+10, y1+10, x2-10, y2-10, fill="black")
                        self.canvas.create_text(center_x, center_y, text="P", fill="white", font=("Arial", 16, "bold"))
                        
                elif self.env.grid[r][c] == 'W':
                    # Wumpus：只有游戏成功完成后或开启作弊模式时才显示
                    if show_element or (cell_visited and (r, c) == self.env.current_pos):
                        wumpus_color = "gray" if not self.env.wumpus_alive else "red"
                        self.canvas.create_oval(x1+5, y1+5, x2-5, y2-5, fill=wumpus_color)
                        self.canvas.create_text(center_x, center_y, text="W", fill="white", font=("Arial", 16, "bold"))
                        
                elif self.env.grid[r][c] == 'G':
                    # 金子：如果已被拾取，则始终不显示；如果在当前位置，则显示；游戏完成后显示所有
                    if not self.env.has_gold:  # 金子还在地图上
                        if show_element or (cell_visited and (r, c) == self.env.current_pos):
                            self.canvas.create_oval(x1+15, y1+15, x2-15, y2-15, fill="gold")
                            self.canvas.create_text(center_x, center_y, text="G", fill="black", font=("Arial", 12, "bold"))
                
                # 为访问过的格子添加浅色边框
                if cell_visited and (r, c) != self.env.current_pos:
                    self.canvas.create_rectangle(x1+2, y1+2, x2-2, y2-2, outline="lightblue", width=2, fill="")
                
                # 绘制智能体
                if (r, c) == self.env.current_pos:
                    # 根据方向绘制箭头
                    if self.env.current_dir == 'North':
                        points = [center_x, y1+10, center_x-20, y2-10, center_x+20, y2-10]
                    elif self.env.current_dir == 'South':
                        points = [center_x, y2-10, center_x-20, y1+10, center_x+20, y1+10]
                    elif self.env.current_dir == 'East':
                        points = [x2-10, center_y, x1+10, center_y-20, x1+10, center_y+20]
                    else:  # West
                        points = [x1+10, center_y, x2-10, center_y-20, x2-10, center_y+20]
                    
                    self.canvas.create_polygon(points, fill="blue", outline="darkblue", width=2)
        
        # 绘制感知提示
        self.draw_percepts()
        
    def draw_percepts(self):
        """绘制感知提示"""
        grid_size = 400 // self.env.size
        percepts = self.env._get_percepts_at_pos(*self.env.current_pos)
        
        r, c = self.env.current_pos
        x1 = c * grid_size
        y1 = (self.env.size - 1 - r) * grid_size
        
        # 在当前格子的角落显示感知
        y_offset = 20
        if 'Stench' in percepts:
            self.canvas.create_text(x1 + 20, y1 + y_offset, text="臭", fill="red", font=("Arial", 10, "bold"))
            y_offset += 15
        if 'Breeze' in percepts:
            self.canvas.create_text(x1 + 20, y1 + y_offset, text="风", fill="green", font=("Arial", 10, "bold"))
            y_offset += 15
        if 'Glitter' in percepts:
            self.canvas.create_text(x1 + 20, y1 + y_offset, text="光", fill="gold", font=("Arial", 10, "bold"))
            y_offset += 15
        if 'Bump' in percepts:
            self.canvas.create_text(x1 + 20, y1 + y_offset, text="撞", fill="orange", font=("Arial", 10, "bold"))
            y_offset += 15
        if 'Scream' in percepts:
            self.canvas.create_text(x1 + 60, y1 + 20, text="叫", fill="purple", font=("Arial", 10, "bold"))
    
    def update_status(self):
        """更新状态信息"""
        self.status_labels["位置"].config(text=str(self.env.current_pos))
        self.status_labels["方向"].config(text=self.env.current_dir)
        self.status_labels["得分"].config(text=str(self.env.score))
        self.status_labels["有箭"].config(text="是" if self.env.has_arrow else "否")
        self.status_labels["有金子"].config(text="是" if self.env.has_gold else "否")
        
        percepts = self.env._get_percepts_at_pos(*self.env.current_pos)
        percept_text = ", ".join(percepts) if percepts else "无"
        self.status_labels["感知"].config(text=percept_text)
    
    def log_message(self, message):
        """添加日志消息"""
        self.log_text.insert(tk.END, f"{message}\\n")
        self.log_text.see(tk.END)
        self.root.update()
    
    def execute_action(self, action):
        """执行动作并更新显示"""
        old_pos = self.env.current_pos
        next_state, reward, done, info = self.env.step(action)
        
        # 记录访问过的格子
        self.visited_cells.add(self.env.current_pos)
        
        self.log_message(f"执行动作: {action}, 奖励: {reward}")
        
        # 特殊事件消息
        if 'death_reason' in info:
            self.log_message(f"游戏结束! 智能体被{info['death_reason']}杀死了!")
            messagebox.showwarning("游戏结束", f"智能体被{info['death_reason']}杀死了!")
        elif 'success' in info and info['success']:
            self.game_completed = True  # 标记游戏成功完成
            self.log_message("恭喜! 智能体成功找到金子并逃脱!")
            self.log_message("现在显示所有隐藏元素的位置!")
            messagebox.showinfo("胜利!", "智能体成功找到金子并逃脱!\n现在可以看到所有隐藏元素的位置了!")
        elif 'grabbed_gold' in info and info['grabbed_gold']:
            self.log_message("智能体抓到了金子!")
        elif 'killed_wumpus' in info and info['killed_wumpus']:
            self.log_message("智能体杀死了Wumpus!")
        elif 'bump' in info and info['bump']:
            self.log_message("智能体撞到了墙!")
        
        self.update_display()
        return done
    
    # 手动控制方法
    def manual_forward(self):
        if not self.is_demo_running:
            self.execute_action('Forward')
    
    def manual_turn_left(self):
        if not self.is_demo_running:
            self.execute_action('TurnLeft')
    
    def manual_turn_right(self):
        if not self.is_demo_running:
            self.execute_action('TurnRight')
    
    def manual_shoot(self):
        if not self.is_demo_running:
            self.execute_action('Shoot')
    
    def manual_grab(self):
        if not self.is_demo_running:
            self.execute_action('Grab')
    
    def manual_climb(self):
        if not self.is_demo_running:
            self.execute_action('Climb')
    
    def create_training_env_from_current(self):
        """创建与当前地图完全相同的训练环境"""
        # 创建新的环境实例
        train_env = WumpusWorldEnv(size=4, num_pits=2, has_wumpus=True, random_seed=None)
        
        # 复制当前环境的地图布局
        train_env.grid = [row[:] for row in self.env.grid]  # 深拷贝网格
        train_env.wumpus_pos = self.env.wumpus_pos
        train_env.gold_pos = self.env.gold_pos
        train_env.pit_positions = self.env.pit_positions[:]  # 复制坑洞位置
        train_env.start_pos = self.env.start_pos
        
        # 重置训练环境的状态
        train_env.reset()
        
        # 记录地图信息
        self.log_message(f"训练地图信息: Wumpus位置{train_env.wumpus_pos}, 金子位置{train_env.gold_pos}, 坑洞位置{train_env.pit_positions}")
        
        return train_env

    def reset_game(self):
        """重置游戏"""
        if not self.is_demo_running:
            if self.use_random_map:
                # 随机地图模式：生成新地图
                seed = None
                self.env = WumpusWorldEnv(size=4, num_pits=2, has_wumpus=True, random_seed=seed)
                self.log_message("游戏已重置（新随机地图）")
            else:
                # 固定地图模式：只重置状态
                self.env.reset()
                self.log_message("游戏已重置（固定地图）")
            
            self.visited_cells.clear()
            self.visited_cells.add(self.env.start_pos)
            self.game_completed = False  # 重置游戏完成标志
            self.update_display()
    
    def start_training(self):
        """开始训练"""
        if self.is_training:
            return
            
        try:
            num_episodes = int(self.episodes_var.get())
            learning_rate = float(self.lr_var.get())
        except ValueError:
            messagebox.showerror("错误", "请输入有效的数字参数")
            return
        
        self.is_training = True
        self.train_button.config(state="disabled")
        self.progress_var.set("训练中...")
        
        # 在新线程中训练以避免界面卡死
        def train_thread():
            try:
                self.log_message(f"开始训练 {num_episodes} 回合...")
                
                # 使用当前GUI中的地图进行训练
                train_env = self.create_training_env_from_current()
                self.log_message("训练环境已创建（使用当前地图），开始Q-learning...")
                
                self.q_table, episode_rewards = optimized_train_agent(
                    train_env,
                    num_episodes=num_episodes,
                    max_steps=100,
                    learning_rate=learning_rate,
                    discount_factor=0.95,
                    epsilon_start=0.9,
                    epsilon_min=0.05,
                    epsilon_decay_rate=0.002,
                    display_interval=max(1, num_episodes // 10)
                )
                
                avg_reward = sum(episode_rewards[-100:]) / min(100, len(episode_rewards))
                
                self.root.after(0, lambda: self.training_complete(avg_reward))
                
            except Exception as e:
                self.root.after(0, lambda: self.training_error(str(e)))
        
        threading.Thread(target=train_thread, daemon=True).start()
    
    def training_complete(self, avg_reward):
        """训练完成回调"""
        self.is_training = False
        self.train_button.config(state="normal")
        self.demo_button.config(state="normal")
        self.progress_var.set(f"训练完成! 平均奖励: {avg_reward:.2f}")
        self.log_message(f"训练完成! 最后100回合平均奖励: {avg_reward:.2f}")
    
    def training_error(self, error_msg):
        """训练错误回调"""
        self.is_training = False
        self.train_button.config(state="normal")
        self.progress_var.set("训练失败")
        self.log_message(f"训练失败: {error_msg}")
        messagebox.showerror("训练失败", f"训练过程中出现错误: {error_msg}")
    
    def start_demo(self):
        """开始AI演示"""
        if self.q_table is None:
            messagebox.showwarning("警告", "请先训练AI模型")
            return
        
        if self.is_demo_running:
            return
        
        self.is_demo_running = True
        self.demo_button.config(state="disabled")
        self.stop_demo_button.config(state="normal")
        
        # 重置环境 - 演示时只重置状态，不改变地图布局
        # 保持当前地图不变，只重置智能体状态
        self.env.reset()
        self.log_message("演示开始：使用当前地图，只重置智能体状态")
        
        self.visited_cells.clear()
        self.visited_cells.add(self.env.start_pos)
        self.game_completed = False  # 重置游戏完成标志
        self.update_display()
        map_type = "随机" if self.use_random_map else "固定"
        self.log_message(f"开始AI演示...（{map_type}地图）")
        
        def demo_thread():
            state = self.env._get_current_state()
            steps = 0
            max_steps = 100
            
            while self.is_demo_running and not self.env.game_over and steps < max_steps:
                # 智能动作选择 - 结合Q表和感知验证
                current_pos = self.env.current_pos
                has_gold = self.env.has_gold
                percepts = self.env._get_percepts_at_pos(*current_pos)
                
                if state in self.q_table:
                    # 获取Q表建议的动作
                    q_values = self.q_table[state].copy()
                    best_action = max(q_values, key=q_values.get)
                    
                    # 验证动作合理性
                    if best_action == 'Grab':
                        if 'Glitter' in percepts and not has_gold:
                            action = 'Grab'  # 合理的抓取
                        else:
                            # 不合理，选择第二好的动作
                            del q_values['Grab']
                            action = max(q_values, key=q_values.get) if q_values else 'Forward'
                    elif best_action == 'Climb':
                        if current_pos == self.env.start_pos:
                            action = 'Climb'  # 合理的攀爬
                        else:
                            # 不在起点，选择其他动作
                            action = 'Forward' if random.uniform(0, 1) < 0.7 else random.choice(['TurnLeft', 'TurnRight'])
                    elif best_action == 'Forward':
                        # 检查前进是否安全
                        if 'Breeze' in percepts and random.uniform(0, 1) < 0.3:
                            # 感知到风险时30%概率转向
                            action = random.choice(['TurnLeft', 'TurnRight'])
                        else:
                            action = 'Forward'
                    else:
                        action = best_action
                else:
                    # 没有Q值，使用基本启发式
                    if 'Glitter' in percepts and not has_gold:
                        action = 'Grab'
                    elif has_gold and current_pos == self.env.start_pos:
                        action = 'Climb'
                    elif 'Breeze' in percepts:
                        action = random.choice(['TurnLeft', 'TurnRight'])
                    else:
                        action = 'Forward'
                
                # 在主线程中执行动作
                self.root.after(0, lambda a=action: self.demo_step(a))
                
                # 等待
                time.sleep(self.speed_var.get())
                steps += 1
                
                # 更新状态
                state = self.env._get_current_state()
            
            # 演示结束
            self.root.after(0, self.demo_complete)
        
        threading.Thread(target=demo_thread, daemon=True).start()
    
    def demo_step(self, action):
        """演示中的单步执行"""
        if self.is_demo_running:
            # 记录决策信息
            current_pos = self.env.current_pos
            has_gold = self.env.has_gold
            percepts = self.env._get_percepts_at_pos(*current_pos)
            
            # 显示AI决策过程
            percept_str = ', '.join(percepts) if percepts else '无'
            self.log_message(f"AI决策: 位置{current_pos}, 感知[{percept_str}], 有金子:{has_gold}, 选择动作:{action}")
            
            done = self.execute_action(action)
            if done:
                self.is_demo_running = False
    
    def demo_complete(self):
        """演示完成"""
        self.is_demo_running = False
        self.demo_button.config(state="normal")
        self.stop_demo_button.config(state="disabled")
        self.log_message("AI演示完成")
    
    def stop_demo(self):
        """停止演示"""
        self.is_demo_running = False
        self.demo_button.config(state="normal")
        self.stop_demo_button.config(state="disabled")
        self.log_message("AI演示已停止")
    
    def toggle_hidden(self):
        """切换隐藏元素显示"""
        self.show_hidden = self.show_hidden_var.get()
        self.update_display()
        if self.show_hidden:
            self.log_message("已开启显示隐藏元素（作弊模式）")
        else:
            self.log_message("已关闭显示隐藏元素（正常模式）")
    
    def toggle_random_map(self):
        """切换随机地图模式"""
        self.use_random_map = self.random_map_var.get()
        if self.use_random_map:
            self.log_message("已开启随机地图模式 - 每次重置都会生成新地图")
        else:
            self.log_message("已关闭随机地图模式 - 使用固定地图")
    
    def generate_new_map(self):
        """生成新地图"""
        if not self.is_demo_running and not self.is_training:
            # 创建新环境
            seed = None if self.use_random_map else 42
            self.env = WumpusWorldEnv(size=4, num_pits=2, has_wumpus=True, random_seed=seed)
            self.visited_cells.clear()
            self.visited_cells.add(self.env.start_pos)
            self.game_completed = False
            self.update_display()
            map_type = "随机" if self.use_random_map else "固定"
            self.log_message(f"已生成新的{map_type}地图")

def main():
    root = tk.Tk()
    app = WumpusGUI(root)
    root.mainloop()

if __name__ == "__main__":
    main()