"""
Wumpus World Online Search Implementation
Wumpus世界在线搜索算法实现

This program implements an online search agent for the Wumpus World problem.
本程序实现了Wumpus世界问题的在线搜索智能体。

Author: AI Assistant
Date: 2024
"""

import random
import time
import os
import sys
from collections import deque

class WumpusWorld:
    """
    Wumpus World环境模拟器
    
    该类负责创建和管理Wumpus世界环境，包括：
    - 世界网格的初始化
    - 元素（Wumpus怪物、金子、陷阱）的放置
    - 智能体感知信息的提供
    - 动作执行和结果反馈
    """
    
    def __init__(self, size=5, num_pits=3, has_wumpus=True, random_seed=None):
        """
        初始化Wumpus世界
        
        参数:
            size (int): 世界网格大小 (size x size)
            num_pits (int): 陷阱数量
            has_wumpus (bool): 是否包含Wumpus怪物
            random_seed (int): 随机种子，用于生成可重复的世界布局
        """
        # 设置随机种子以确保可重复性
        if random_seed is not None:
            random.seed(random_seed)
            
        # 世界基本属性
        self.size = size  # 世界大小
        self.grid = [[' ' for _ in range(size)] for _ in range(size)]  # 网格，空白表示空地
        
        # 智能体状态
        self.agent_pos = (0, 0)  # 智能体位置，从左下角(0,0)开始
        self.agent_dir = 'East'  # 智能体初始朝向：东
        self.has_arrow = True  # 智能体是否有箭
        self.has_gold = False  # 智能体是否持有金子
        
        # Wumpus怪物状态
        self.wumpus_alive = has_wumpus  # Wumpus是否活着
        self.wumpus_pos = None  # Wumpus位置
        
        # 游戏状态
        self.game_over = False  # 游戏是否结束
        self.score = 0  # 当前得分
        self.actions_taken = 0  # 已执行的动作数
        self.gold_pos = None  # 金子位置
        
        # 放置世界元素
        self._place_elements(num_pits, has_wumpus)
        
    def _place_elements(self, num_pits, has_wumpus):
        """
        在世界中放置各种元素
        
        放置规则：
        - 起始位置(0,0)必须安全
        - 金子、Wumpus、陷阱不能放在起始位置
        - 各元素不能重叠
        
        参数:
            num_pits (int): 要放置的陷阱数量
            has_wumpus (bool): 是否放置Wumpus
        """
        # 确保起始位置安全，标记为起点
        self.grid[0][0] = 'S'  # S表示Start起点
        
        # 放置金子（不能在起点）
        while True:
            r, c = random.randint(0, self.size-1), random.randint(0, self.size-1)
            if (r, c) != (0, 0):  # 确保不在起点
                self.grid[r][c] = 'G'  # G表示Gold金子
                self.gold_pos = (r, c)
                break
        
        # 放置Wumpus怪物（不能在起点，不能与金子重叠）
        if has_wumpus:
            while True:
                r, c = random.randint(0, self.size-1), random.randint(0, self.size-1)
                if (r, c) != (0, 0) and self.grid[r][c] == ' ':  # 确保不在起点且位置空闲
                    self.grid[r][c] = 'W'  # W表示Wumpus
                    self.wumpus_pos = (r, c)
                    break
        
        # 放置陷阱（不能在起点，不能与其他元素重叠）
        pits_placed = 0
        while pits_placed < num_pits:
            r, c = random.randint(0, self.size-1), random.randint(0, self.size-1)
            if (r, c) != (0, 0) and self.grid[r][c] == ' ':  # 确保不在起点且位置空闲
                self.grid[r][c] = 'P'  # P表示Pit陷阱
                pits_placed += 1

    def get_percepts(self):
        """
        获取智能体当前位置的感知信息
        
        感知规则：
        - Glitter（闪光）：当前位置有金子
        - Stench（恶臭）：相邻位置有活着的Wumpus
        - Breeze（微风）：相邻位置有陷阱
        
        返回:
            list: 当前位置的感知信息列表
        """
        percepts = []  # 感知信息列表
        r, c = self.agent_pos  # 获取智能体当前位置
        
        # 检查当前位置是否有金子
        if 'G' in self.grid[r][c]:
            percepts.append('Glitter')  # 添加闪光感知
        
        # 检查相邻位置是否有Wumpus或陷阱
        neighbors = self._get_neighbors(r, c)  # 获取相邻位置
        
        for nr, nc in neighbors:
            # 确保相邻位置在网格范围内
            if 0 <= nr < self.size and 0 <= nc < self.size:
                # 检查是否有活着的Wumpus
                if 'W' in self.grid[nr][nc] and self.wumpus_alive:
                    percepts.append('Stench')  # 添加恶臭感知
                # 检查是否有陷阱
                if 'P' in self.grid[nr][nc]:
                    percepts.append('Breeze')  # 添加微风感知
        
        return percepts

    def _get_neighbors(self, r, c):
        """
        获取指定位置的四个相邻位置
        
        参数:
            r, c (int): 位置坐标
            
        返回:
            list: 四个相邻位置的坐标列表 [(r,c), ...]
        """
        return [(r+1, c), (r-1, c), (r, c+1), (r, c-1)]  # 上下左右四个方向

    def execute_action(self, action):
        """
        执行智能体的动作并返回结果
        
        可执行的动作：
        - Forward: 向前移动
        - TurnLeft: 向左转
        - TurnRight: 向右转
        - Shoot: 射箭
        - Grab: 抓取金子
        - Climb: 爬出世界
        
        参数:
            action (str): 要执行的动作
            
        返回:
            list: 动作执行结果的描述
        """
        self.actions_taken += 1  # 增加动作计数
        self.score -= 1  # 每个动作消耗1分（行动成本）
        
        if action == 'Forward':
            # 向前移动
            new_r, new_c = self._get_next_pos()  # 计算移动后的位置
            if self._is_valid(new_r, new_c):  # 检查新位置是否在网格内
                self.agent_pos = (new_r, new_c)  # 更新智能体位置
                r, c = new_r, new_c
                
                # 检查新位置是否有Wumpus
                if 'W' in self.grid[r][c] and self.wumpus_alive:
                    self.game_over = True
                    self.score -= 1000  # 被Wumpus吃掉的惩罚
                    return ['You were eaten by the Wumpus!']
                
                # 检查新位置是否有陷阱
                if 'P' in self.grid[r][c]:
                    self.game_over = True
                    self.score -= 1000  # 掉入陷阱的惩罚
                    return ['You fell into a pit!']
                
                return ['Moved forward']  # 成功移动
            else:
                return ['Bump']  # 撞墙了
        
        elif action == 'TurnLeft':
            # 向左转（逆时针90度）
            directions = ['North', 'West', 'South', 'East']  # 逆时针方向序列
            current_idx = directions.index(self.agent_dir)  # 找到当前方向的索引
            self.agent_dir = directions[(current_idx + 1) % 4]  # 转到下一个方向
            return ['Turned left']
        
        elif action == 'TurnRight':
            # 向右转（顺时针90度）
            directions = ['North', 'East', 'South', 'West']  # 顺时针方向序列
            current_idx = directions.index(self.agent_dir)  # 找到当前方向的索引
            self.agent_dir = directions[(current_idx + 1) % 4]  # 转到下一个方向
            return ['Turned right']
        
        elif action == 'Shoot':
            # 射箭
            if not self.has_arrow:
                return ['No arrow left']  # 没有箭了
            
            self.has_arrow = False  # 消耗箭
            self.score -= 10  # 射箭额外消耗10分
            
            # 检查箭是否击中Wumpus
            r, c = self.agent_pos
            
            # 根据朝向确定箭的飞行路径
            if self.agent_dir == 'North':
                while r < self.size:
                    r += 1
                    if 0 <= r < self.size and 'W' in self.grid[r][c]:
                        self.wumpus_alive = False  # Wumpus被杀死
                        return ['Scream']  # Wumpus的惨叫声
            elif self.agent_dir == 'South':
                while r >= 0:
                    r -= 1
                    if 0 <= r < self.size and 'W' in self.grid[r][c]:
                        self.wumpus_alive = False
                        return ['Scream']
            elif self.agent_dir == 'East':
                while c < self.size:
                    c += 1
                    if 0 <= c < self.size and 'W' in self.grid[r][c]:
                        self.wumpus_alive = False
                        return ['Scream']
            elif self.agent_dir == 'West':
                while c >= 0:
                    c -= 1
                    if 0 <= c < self.size and 'W' in self.grid[r][c]:
                        self.wumpus_alive = False
                        return ['Scream']
            
            return ['Arrow missed']  # 箭没有击中目标
        
        elif action == 'Grab':
            # 抓取金子
            r, c = self.agent_pos
            if 'G' in self.grid[r][c]:  # 检查当前位置是否有金子
                self.has_gold = True  # 获得金子
                self.grid[r][c] = self.grid[r][c].replace('G', ' ')  # 从网格中移除金子
                self.score += 1000  # 获得金子的奖励
                return ['Gold grabbed']
            else:
                return ['No gold here']  # 这里没有金子
        
        elif action == 'Climb':
            # 爬出世界（只能在起点进行）
            r, c = self.agent_pos
            if (r, c) == (0, 0):  # 检查是否在起点
                if self.has_gold:
                    self.game_over = True
                    self.score += 1000  # 带着金子逃脱的额外奖励
                    return ['Climbed out with gold!']
                else:
                    self.game_over = True
                    return ['Climbed out without gold...']
            else:
                return ['Cannot climb from here']  # 只能在起点爬出
        
        return ['Invalid action']  # 无效动作

    def _get_next_pos(self):
        """
        根据当前朝向计算向前移动后的位置
        
        返回:
            tuple: 移动后的位置坐标 (row, col)
        """
        r, c = self.agent_pos
        if self.agent_dir == 'North':
            return (r + 1, c)  # 向北（上）移动
        elif self.agent_dir == 'South':
            return (r - 1, c)  # 向南（下）移动
        elif self.agent_dir == 'East':
            return (r, c + 1)  # 向东（右）移动
        elif self.agent_dir == 'West':
            return (r, c - 1)  # 向西（左）移动
        return (r, c)  # 默认不移动

    def _is_valid(self, r, c):
        """
        检查位置是否在网格边界内
        
        参数:
            r, c (int): 要检查的位置坐标
            
        返回:
            bool: 位置是否有效
        """
        return 0 <= r < self.size and 0 <= c < self.size

    def display_world(self, show_all=False, agent_kb=None):
        """
        显示世界地图
        
        参数:
            show_all (bool): 是否显示完整地图（调试用）
            agent_kb (dict): 智能体的知识库，用于显示已知信息
        """
        print("-" * (self.size * 4 + 1))  # 打印顶部边框
        for r in range(self.size-1, -1, -1):  # 从上到下打印（网格坐标从下到上）
            line = "|"
            for c in range(self.size):
                cell_content = ' '  # 默认空白
                
                # 根据访问状态显示单元格内容
                if show_all or (agent_kb and agent_kb['visited'][r][c]):
                    cell = self.grid[r][c]
                    if 'P' in cell:
                        cell_content = 'P'  # 陷阱
                    elif 'W' in cell and self.wumpus_alive:
                        cell_content = 'W'  # 活着的Wumpus
                    elif 'G' in cell:
                        cell_content = 'G'  # 金子
                    elif 'S' in cell:
                        cell_content = 'S'  # 起点
                
                # 显示智能体位置和朝向
                if (r, c) == self.agent_pos:
                    if self.agent_dir == 'North':
                        cell_content = '^'  # 朝北
                    elif self.agent_dir == 'South':
                        cell_content = 'v'  # 朝南
                    elif self.agent_dir == 'East':
                        cell_content = '>'  # 朝东
                    elif self.agent_dir == 'West':
                        cell_content = '<'  # 朝西
                
                # 显示智能体的知识推理结果
                if not show_all and agent_kb:
                    if agent_kb['visited'][r][c]:
                        pass  # 保持实际内容
                    elif agent_kb['safe'][r][c]:
                        cell_content = '.'  # 安全但未访问
                    elif agent_kb['wumpus_possible'][r][c] and agent_kb['pit_possible'][r][c]:
                        cell_content = '?'  # 可能有两种危险
                    elif agent_kb['wumpus_possible'][r][c]:
                        cell_content = 'w'  # 可能有Wumpus
                    elif agent_kb['pit_possible'][r][c]:
                        cell_content = 'p'  # 可能有陷阱
                
                line += f" {cell_content} |"
            print(line)
            print("-" * (self.size * 4 + 1))  # 打印行分隔线
        
        # 打印状态信息
        print(f"Arrow: {'Yes' if self.has_arrow else 'No'} | Gold: {'Yes' if self.has_gold else 'No'} | Score: {self.score}")


class Agent:
    """
    Wumpus世界在线搜索智能体
    
    该智能体使用在线搜索算法来探索Wumpus世界：
    - 维护知识库记录已知信息
    - 使用逻辑推理更新世界状态
    - 采用安全优先的探索策略
    - 支持路径规划和回溯
    """
    
    def __init__(self, world_size):
        """
        初始化智能体
        
        参数:
            world_size (int): 世界网格大小
        """
        self.world_size = world_size
        
        # 知识库 - 记录智能体对世界的认知
        self.knowledge_base = {
            'visited': [[False for _ in range(world_size)] for _ in range(world_size)],  # 已访问的位置
            'safe': [[False for _ in range(world_size)] for _ in range(world_size)],  # 确认安全的位置
            'wumpus_possible': [[False for _ in range(world_size)] for _ in range(world_size)],  # 可能有Wumpus的位置
            'pit_possible': [[False for _ in range(world_size)] for _ in range(world_size)],  # 可能有陷阱的位置
            'path': []  # 路径历史，用于回溯
        }
        
        # 智能体当前状态
        self.current_pos = (0, 0)  # 当前位置
        self.current_dir = 'East'  # 当前朝向
        self.has_arrow = True  # 是否有箭
        self.has_gold = False  # 是否有金子
        
        # 行动规划
        self.plan = []  # 当前执行计划
        self.frontier = []  # 探索边界（安全但未访问的位置）
        
        # 将起始位置标记为安全且已访问
        self.knowledge_base['safe'][0][0] = True
        self.knowledge_base['visited'][0][0] = True

    def update_knowledge_base(self, percepts):
        """
        根据当前感知更新知识库
        
        智能体通过逻辑推理分析感知信息：
        - 没有微风 → 相邻格子无陷阱
        - 有微风 → 相邻格子可能有陷阱
        - 没有恶臭 → 相邻格子无Wumpus
        - 有恶臭 → 相邻格子可能有Wumpus
        - 有闪光 → 当前位置有金子
        
        参数:
            percepts (list): 当前位置的感知信息列表
        """
        r, c = self.current_pos
        # 标记当前位置为已访问和安全
        self.knowledge_base['visited'][r][c] = True
        self.knowledge_base['safe'][r][c] = True
        
        # 获取相邻位置
        neighbors = self._get_neighbors(r, c)
        
        # 分析微风感知 - 推理陷阱位置
        if 'Breeze' not in percepts:
            # 没有微风 → 所有相邻位置都没有陷阱
            for nr, nc in neighbors:
                if self._is_valid(nr, nc):
                    self.knowledge_base['pit_possible'][nr][nc] = False
                    # 如果也没有Wumpus可能，则标记为安全
                    if not self.knowledge_base['wumpus_possible'][nr][nc]:
                        self.knowledge_base['safe'][nr][nc] = True
        else:
            # 有微风 → 相邻位置可能有陷阱（仅标记未访问的位置）
            for nr, nc in neighbors:
                if self._is_valid(nr, nc) and not self.knowledge_base['visited'][nr][nc]:
                    self.knowledge_base['pit_possible'][nr][nc] = True
        
        # 分析恶臭感知 - 推理Wumpus位置
        if 'Stench' not in percepts:
            # 没有恶臭 → 所有相邻位置都没有Wumpus
            for nr, nc in neighbors:
                if self._is_valid(nr, nc):
                    self.knowledge_base['wumpus_possible'][nr][nc] = False
                    # 如果也没有陷阱可能，则标记为安全
                    if not self.knowledge_base['pit_possible'][nr][nc]:
                        self.knowledge_base['safe'][nr][nc] = True
        else:
            # 有恶臭 → 相邻位置可能有Wumpus（仅标记未访问的位置）
            for nr, nc in neighbors:
                if self._is_valid(nr, nc) and not self.knowledge_base['visited'][nr][nc]:
                    self.knowledge_base['wumpus_possible'][nr][nc] = True
        
        # 分析闪光感知 - 发现金子
        if 'Glitter' in percepts:
            self.plan = ['Grab']  # 计划抓取金子
        
        # 记录路径历史（用于回溯）
        if not self.knowledge_base['path'] or self.knowledge_base['path'][-1] != self.current_pos:
            self.knowledge_base['path'].append(self.current_pos)
        
        # 更新探索边界
        self._update_frontier()

    def _update_frontier(self):
        """
        更新探索边界
        
        边界是指那些已知安全但未访问的位置，这些位置是下一步探索的候选目标。
        使用曼哈顿距离作为优先级，优先探索距离较近的位置。
        """
        self.frontier = []
        for r in range(self.world_size):
            for c in range(self.world_size):
                if self.knowledge_base['safe'][r][c] and not self.knowledge_base['visited'][r][c]:
                    # 计算曼哈顿距离作为优先级
                    distance = abs(r - self.current_pos[0]) + abs(c - self.current_pos[1])
                    self.frontier.append(((r, c), distance))
        
        # 按距离排序（近的优先）
        self.frontier.sort(key=lambda x: x[1])

    def choose_action(self):
        """
        使用在线搜索算法选择下一个动作
        
        决策优先级：
        1. 执行现有计划（如抓取金子）
        2. 如果有金子且在起点，爬出世界
        3. 如果有金子，返回起点
        4. 探索相邻的安全未访问位置
        5. 前往边界位置探索
        6. 射击可能的Wumpus
        7. 回溯到之前的位置
        8. 冒险探索可能安全的位置
        9. 随机转向
        
        返回:
            str: 要执行的动作
        """
        # 1. 如果有计划，优先执行
        if self.plan:
            return self.plan.pop(0)
        
        # 2. 如果有金子且在起点，爬出世界
        if self.has_gold and self.current_pos == (0, 0):
            return 'Climb'
        
        # 3. 如果有金子，计划返回起点
        if self.has_gold:
            action = self._plan_path_to((0, 0))
            if action:
                return action
        
        # 4. 寻找相邻的安全未访问位置（局部探索）
        r, c = self.current_pos
        neighbors = self._get_neighbors(r, c)
        safe_unvisited = []
        
        for nr, nc in neighbors:
            if self._is_valid(nr, nc) and self.knowledge_base['safe'][nr][nc] and not self.knowledge_base['visited'][nr][nc]:
                safe_unvisited.append((nr, nc))
        
        # 如果有安全的相邻位置，移动过去
        if safe_unvisited:
            target = random.choice(safe_unvisited)
            return self._get_action_to_move(target)
        
        # 5. 如果有边界位置，计划前往最近的边界
        if self.frontier:
            target, _ = self.frontier[0]  # 最近的边界位置
            action = self._plan_path_to(target)
            if action:
                return action
        
        # 6. 如果有箭且没有明确目标，考虑射击Wumpus
        if self.has_arrow:
            for r in range(self.world_size):
                for c in range(self.world_size):
                    if self.knowledge_base['wumpus_possible'][r][c] and not self.knowledge_base['visited'][r][c]:
                        # 检查Wumpus是否在同一行或列（可以射击）
                        cr, cc = self.current_pos
                        if r == cr or c == cc:
                            # 转向面对Wumpus
                            if r > cr and self.current_dir != 'North':
                                return 'TurnRight' if self.current_dir == 'East' else 'TurnLeft'
                            elif r < cr and self.current_dir != 'South':
                                return 'TurnRight' if self.current_dir == 'West' else 'TurnLeft'
                            elif c > cc and self.current_dir != 'East':
                                return 'TurnRight' if self.current_dir == 'North' else 'TurnLeft'
                            elif c < cc and self.current_dir != 'West':
                                return 'TurnRight' if self.current_dir == 'South' else 'TurnLeft'
                            else:
                                return 'Shoot'  # 已经面对Wumpus，射击
        
        # 7. 如果没有明确的安全移动，考虑回溯
        if len(self.knowledge_base['path']) > 1:
            # 获取不是当前位置的前一个位置
            prev_pos = None
            for pos in reversed(self.knowledge_base['path'][:-1]):
                if pos != self.current_pos:
                    prev_pos = pos
                    break
            
            if prev_pos:
                # 尝试回到前一个位置
                return self._get_action_to_move(prev_pos)
        
        # 8. 作为最后手段，冒险探索可能安全的位置
        risky_moves = []
        for nr, nc in self._get_neighbors(*self.current_pos):
            if self._is_valid(nr, nc) and not self.knowledge_base['visited'][nr][nc]:
                # 计算风险（简单模型）
                risk = 0
                if self.knowledge_base['pit_possible'][nr][nc]:
                    risk += 1  # 可能有陷阱
                if self.knowledge_base['wumpus_possible'][nr][nc]:
                    risk += 1  # 可能有Wumpus
                risky_moves.append(((nr, nc), risk))
        
        if risky_moves:
            # 按风险排序（低风险优先）
            risky_moves.sort(key=lambda x: x[1])
            target, _ = risky_moves[0]
            return self._get_action_to_move(target)
        
        # 9. 如果所有方法都失败，随机转向
        return random.choice(['TurnLeft', 'TurnRight'])

    def _get_neighbors(self, r, c):
        """
        获取指定位置的四个相邻位置
        
        参数:
            r, c (int): 位置坐标
            
        返回:
            list: 四个相邻位置的坐标列表 [(r,c), ...]
        """
        return [(r+1, c), (r-1, c), (r, c+1), (r, c-1)]  # 上下左右

    def _is_valid(self, r, c):
        """
        检查位置是否在网格边界内
        
        参数:
            r, c (int): 要检查的位置坐标
            
        返回:
            bool: 位置是否有效
        """
        return 0 <= r < self.world_size and 0 <= c < self.world_size

    def _get_action_to_move(self, target):
        """
        获取移动到目标位置的动作
        
        仅处理相邻位置的移动，根据当前朝向和目标方向决定动作。
        注意：需要两次转向的情况下，这里只返回第一次转向。
        
        参数:
            target (tuple): 目标位置 (row, col)
            
        返回:
            str: 要执行的动作
        """
        tr, tc = target
        r, c = self.current_pos
        
        # 目标在北方（上方）
        if tr > r and tc == c:
            if self.current_dir == 'North':
                return 'Forward'
            elif self.current_dir == 'East':
                return 'TurnLeft'
            elif self.current_dir == 'South':
                return 'TurnRight'  # 需要转两次，先转一次
            else:  # West
                return 'TurnRight'
        
        # 目标在南方（下方）
        elif tr < r and tc == c:
            if self.current_dir == 'South':
                return 'Forward'
            elif self.current_dir == 'East':
                return 'TurnRight'
            elif self.current_dir == 'North':
                return 'TurnRight'  # 需要转两次，先转一次
            else:  # West
                return 'TurnLeft'
        
        # 目标在东方（右方）
        elif tr == r and tc > c:
            if self.current_dir == 'East':
                return 'Forward'
            elif self.current_dir == 'North':
                return 'TurnRight'
            elif self.current_dir == 'South':
                return 'TurnLeft'
            else:  # West
                return 'TurnRight'  # 需要转两次，先转一次
        
        # 目标在西方（左方）
        elif tr == r and tc < c:
            if self.current_dir == 'West':
                return 'Forward'
            elif self.current_dir == 'North':
                return 'TurnLeft'
            elif self.current_dir == 'South':
                return 'TurnRight'
            else:  # East
                return 'TurnRight'  # 需要转两次，先转一次
        
        return 'TurnLeft'  # 默认动作

    def _find_path_to(self, target):
        """
        使用广度优先搜索找到从当前位置到目标的路径
        
        仅通过安全的位置进行路径搜索，确保移动的安全性。
        
        参数:
            target (tuple): 目标位置 (row, col)
            
        返回:
            list: 路径列表，包含从当前位置到目标的所有中间位置
        """
        start = self.current_pos
        queue = deque([(start, [])])  # (当前位置, 路径)
        visited = set([start])
        
        while queue:
            (r, c), path = queue.popleft()
            
            if (r, c) == target:
                return path
            
            # 探索相邻的安全位置
            for nr, nc in self._get_neighbors(r, c):
                if self._is_valid(nr, nc) and (nr, nc) not in visited and self.knowledge_base['safe'][nr][nc]:
                    visited.add((nr, nc))
                    queue.append(((nr, nc), path + [(nr, nc)]))
        
        return []  # 没有找到路径

    def _plan_path_to(self, target):
        """
        规划到目标位置的路径并返回第一个动作
        
        使用广度优先搜索在安全位置构成的图中寻找最短路径，
        然后返回路径上第一步需要执行的动作。
        
        参数:
            target (tuple): 目标位置 (row, col)
            
        返回:
            str or None: 下一步要执行的动作，如果无法到达则返回None
        """
        # 创建安全位置的简化图
        graph = {}
        for r in range(self.world_size):
            for c in range(self.world_size):
                if self.knowledge_base['safe'][r][c]:
                    neighbors = []
                    for nr, nc in self._get_neighbors(r, c):
                        if self._is_valid(nr, nc) and self.knowledge_base['safe'][nr][nc]:
                            neighbors.append((nr, nc))
                    graph[(r, c)] = neighbors
        
        # 使用BFS寻找路径
        start = self.current_pos
        if start not in graph or target not in graph:
            return None  # 起点或终点不安全
        
        queue = deque([(start, [])])
        visited = {start}
        
        while queue:
            node, path = queue.popleft()
            
            if node == target:
                if path:
                    next_pos = path[0]  # 路径上的下一个位置
                    return self._get_action_to_move(next_pos)
                return None  # 已经在目标位置
            
            for neighbor in graph.get(node, []):
                if neighbor not in visited:
                    visited.add(neighbor)
                    queue.append((neighbor, path + [neighbor]))
        
        return None  # 没有找到路径

    def set_agent_state(self, pos, direction, has_arrow, has_gold):
        """
        更新智能体的内部状态
        
        用于同步智能体的状态与环境状态，确保一致性。
        
        参数:
            pos (tuple): 智能体位置 (row, col)
            direction (str): 智能体朝向
            has_arrow (bool): 是否有箭
            has_gold (bool): 是否有金子
        """
        self.current_pos = pos
        self.current_dir = direction
        self.has_arrow = has_arrow
        self.has_gold = has_gold


def clear_screen():
    """
    清除控制台屏幕
    
    根据操作系统类型选择合适的清屏命令
    """
    os.system('cls' if os.name == 'nt' else 'clear')


def wait_for_space():
    """
    等待用户按下空格键继续或输入'q'退出
    
    提供单步控制功能，让用户可以逐步观察智能体的行为。
    
    返回:
        bool: True表示继续，False表示退出
    """
    print("\n按 [空格键] 继续下一步，按 'q' 退出程序...")
    while True:
        try:
            user_input = input().strip()
            if user_input == '':  # 空格键产生空字符串
                return True
            elif user_input.lower() == 'q':
                return False
            else:
                print("请按 [空格键] 继续，或者输入 'q' 退出...")
        except KeyboardInterrupt:
            return False


def main():
    """Main function to run the Wumpus World simulation"""
    # Parse command line arguments
    world_size = 4
    num_pits = 3
    has_wumpus = True
    random_seed = None
    max_steps = 100  # 添加最大步数限制
    
    # Process command line arguments
    if len(sys.argv) > 1:
        for i in range(1, len(sys.argv)):
            if sys.argv[i] == '--size' and i+1 < len(sys.argv):
                world_size = int(sys.argv[i+1])
            elif sys.argv[i] == '--pits' and i+1 < len(sys.argv):
                num_pits = int(sys.argv[i+1])
            elif sys.argv[i] == '--no-wumpus':
                has_wumpus = False
            elif sys.argv[i] == '--seed' and i+1 < len(sys.argv):
                random_seed = int(sys.argv[i+1])
            elif sys.argv[i] == '--max-steps' and i+1 < len(sys.argv):
                max_steps = int(sys.argv[i+1])
    
    # Initialize world and agent
    world = WumpusWorld(world_size, num_pits, has_wumpus, random_seed)
    agent = Agent(world_size)
    
    # Print initial information
    clear_screen()
    print("\n=== Wumpus World - 单步控制模式 ===")
    print("Size:", world_size, "x", world_size)
    print("Pits:", num_pits)
    print("Wumpus:", "Yes" if has_wumpus else "No")
    print("\nLegend:")
    print("  > ^ < v : Agent and direction")
    print("  W : Wumpus")
    print("  P : Pit")
    print("  G : Gold")
    print("  . : Safe unvisited")
    print("  w : Possible Wumpus")
    print("  p : Possible pit")
    print("  ? : Danger (both possible)")
    print("\n控制说明:")
    print("  按 [空格键] 让智能体执行下一步")
    print("  输入 'q' 退出程序")
    print("\nPress Enter to start...")
    input()
    
    # Game loop
    step = 0
    try:
        while not world.game_over:
            step += 1
            
            # 检查是否超过最大步数限制
            if step > max_steps:
                print(f"\n已达到最大步数 {max_steps}，可能陷入循环，程序自动退出")
                break
            
            # Clear screen and display world
            clear_screen()
            print(f"\n=== Wumpus World - 步骤 {step}/{max_steps} ===")
            world.display_world(show_all=False, agent_kb=agent.knowledge_base)
            
            # Get percepts
            percepts = world.get_percepts()
            print(f"\n感知信息: {', '.join(percepts) if percepts else 'None'}")
            
            # Agent updates knowledge base
            agent.update_knowledge_base(percepts)
            
            # Agent chooses action
            action = agent.choose_action()
            print(f"智能体计划执行: {action}")
            
            # Wait for user to press space
            if not wait_for_space():
                raise KeyboardInterrupt
            
            # Execute action
            result = world.execute_action(action)
            print(f"执行结果: {', '.join(result)}")
            
            # Update agent's state
            agent.set_agent_state(world.agent_pos, world.agent_dir, world.has_arrow, world.has_gold)
            
            # Small delay to show result
            time.sleep(0.5)
    
    except KeyboardInterrupt:
        print("\n用户中断，程序退出")
    
    # Game over
    clear_screen()
    print("\n=== 游戏结束 ===")
    world.display_world(show_all=True)
    print(f"\n最终得分: {world.score}")
    print(f"总步数: {world.actions_taken}")
    
    if world.has_gold and world.agent_pos == (0, 0):
        print("成功！智能体找到了金子并成功逃脱！")
    elif world.has_gold:
        print("智能体找到了金子但未能逃脱！")
    else:
        print("智能体未能找到金子。")
    
    # Print cause of failure if applicable
    if world.agent_pos != (0, 0) or not world.has_gold:
        r, c = world.agent_pos
        if 'W' in world.grid[r][c] and world.wumpus_alive:
            print("智能体被Wumpus吃掉了！")
        elif 'P' in world.grid[r][c]:
            print("智能体掉进了坑洞！")


if __name__ == "__main__":
    main() 