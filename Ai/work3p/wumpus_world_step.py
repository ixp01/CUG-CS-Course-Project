import random
import time
import os
import sys
from collections import deque

class WumpusWorld:
    """Wumpus World environment simulation"""
    
    def __init__(self, size=4, num_pits=3, has_wumpus=True, random_seed=None):
        """Initialize the Wumpus World"""
        if random_seed is not None:
            random.seed(random_seed)
            
        self.size = size
        self.grid = [[' ' for _ in range(size)] for _ in range(size)]
        self.agent_pos = (0, 0)  # Start at bottom-left (0,0)
        self.agent_dir = 'East'  # Initial direction facing east
        self.has_arrow = True
        self.has_gold = False
        self.wumpus_alive = has_wumpus
        self.wumpus_pos = None
        self.game_over = False
        self.score = 0
        self.actions_taken = 0
        self.gold_pos = None
        self._place_elements(num_pits, has_wumpus)
        
    def _place_elements(self, num_pits, has_wumpus):
        """Place Wumpus, pits, and gold in the world"""
        # Ensure starting position is safe
        self.grid[0][0] = 'S'  # Start
        
        # Place gold (not at start)
        while True:
            r, c = random.randint(0, self.size-1), random.randint(0, self.size-1)
            if (r, c) != (0, 0):
                self.grid[r][c] = 'G'  # Gold
                self.gold_pos = (r, c)
                break
        
        # Place Wumpus (not at start)
        if has_wumpus:
            while True:
                r, c = random.randint(0, self.size-1), random.randint(0, self.size-1)
                if (r, c) != (0, 0) and self.grid[r][c] == ' ':
                    self.grid[r][c] = 'W'  # Wumpus
                    self.wumpus_pos = (r, c)
                    break
        
        # Place pits (not at start)
        pits_placed = 0
        while pits_placed < num_pits:
            r, c = random.randint(0, self.size-1), random.randint(0, self.size-1)
            if (r, c) != (0, 0) and self.grid[r][c] == ' ':
                self.grid[r][c] = 'P'  # Pit
                pits_placed += 1

    def get_percepts(self):
        """Return percepts at the agent's current location"""
        percepts = []
        r, c = self.agent_pos
        
        # Check for gold
        if 'G' in self.grid[r][c]:
            percepts.append('Glitter')
        
        # Check for Wumpus or pits in adjacent cells
        neighbors = self._get_neighbors(r, c)
        
        for nr, nc in neighbors:
            if 0 <= nr < self.size and 0 <= nc < self.size:
                if 'W' in self.grid[nr][nc] and self.wumpus_alive:
                    percepts.append('Stench')
                if 'P' in self.grid[nr][nc]:
                    percepts.append('Breeze')
        
        return percepts

    def _get_neighbors(self, r, c):
        """Get the four adjacent cells"""
        return [(r+1, c), (r-1, c), (r, c+1), (r, c-1)]

    def execute_action(self, action):
        """Execute an action and return the result"""
        self.actions_taken += 1
        self.score -= 1  # Each action costs energy
        
        if action == 'Forward':
            new_r, new_c = self._get_next_pos()
            if self._is_valid(new_r, new_c):
                self.agent_pos = (new_r, new_c)
                r, c = new_r, new_c
                
                # Check for Wumpus
                if 'W' in self.grid[r][c] and self.wumpus_alive:
                    self.game_over = True
                    self.score -= 1000
                    return ['You were eaten by the Wumpus!']
                
                # Check for pit
                if 'P' in self.grid[r][c]:
                    self.game_over = True
                    self.score -= 1000
                    return ['You fell into a pit!']
                
                return ['Moved forward']
            else:
                return ['Bump']  # Hit a wall
        
        elif action == 'TurnLeft':
            directions = ['North', 'West', 'South', 'East']
            current_idx = directions.index(self.agent_dir)
            self.agent_dir = directions[(current_idx + 1) % 4]
            return ['Turned left']
        
        elif action == 'TurnRight':
            directions = ['North', 'East', 'South', 'West']
            current_idx = directions.index(self.agent_dir)
            self.agent_dir = directions[(current_idx + 1) % 4]
            return ['Turned right']
        
        elif action == 'Shoot':
            if not self.has_arrow:
                return ['No arrow left']
            
            self.has_arrow = False
            self.score -= 10  # Shooting costs extra energy
            
            # Check if arrow hit Wumpus
            r, c = self.agent_pos
            if self.agent_dir == 'North':
                while r < self.size:
                    r += 1
                    if 0 <= r < self.size and 'W' in self.grid[r][c]:
                        self.wumpus_alive = False
                        return ['Scream']
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
            
            return ['Arrow missed']
        
        elif action == 'Grab':
            r, c = self.agent_pos
            if 'G' in self.grid[r][c]:
                self.has_gold = True
                self.grid[r][c] = self.grid[r][c].replace('G', ' ')
                self.score += 1000  # Gold reward
                return ['Gold grabbed']
            else:
                return ['No gold here']
        
        elif action == 'Climb':
            r, c = self.agent_pos
            if (r, c) == (0, 0):
                if self.has_gold:
                    self.game_over = True
                    self.score += 1000  # Escape reward
                    return ['Climbed out with gold!']
                else:
                    self.game_over = True
                    return ['Climbed out without gold...']
            else:
                return ['Cannot climb from here']
        
        return ['Invalid action']

    def _get_next_pos(self):
        """Get position after moving forward"""
        r, c = self.agent_pos
        if self.agent_dir == 'North':
            return (r + 1, c)
        elif self.agent_dir == 'South':
            return (r - 1, c)
        elif self.agent_dir == 'East':
            return (r, c + 1)
        elif self.agent_dir == 'West':
            return (r, c - 1)
        return (r, c)

    def _is_valid(self, r, c):
        """Check if position is within grid bounds"""
        return 0 <= r < self.size and 0 <= c < self.size

    def display_world(self, show_all=False, agent_kb=None):
        """Display the world map"""
        print("-" * (self.size * 4 + 1))
        for r in range(self.size-1, -1, -1):  # Print from top to bottom
            line = "|"
            for c in range(self.size):
                cell_content = ' '
                
                # Show cell content if visible
                if show_all or (agent_kb and agent_kb['visited'][r][c]):
                    cell = self.grid[r][c]
                    if 'P' in cell:
                        cell_content = 'P'
                    elif 'W' in cell and self.wumpus_alive:
                        cell_content = 'W'
                    elif 'G' in cell:
                        cell_content = 'G'
                    elif 'S' in cell:
                        cell_content = 'S'
                
                # Show agent
                if (r, c) == self.agent_pos:
                    if self.agent_dir == 'North':
                        cell_content = '^'
                    elif self.agent_dir == 'South':
                        cell_content = 'v'
                    elif self.agent_dir == 'East':
                        cell_content = '>'
                    elif self.agent_dir == 'West':
                        cell_content = '<'
                
                # Show agent's knowledge if available
                if not show_all and agent_kb:
                    if agent_kb['visited'][r][c]:
                        pass  # Keep the actual content
                    elif agent_kb['safe'][r][c]:
                        cell_content = '.'  # Safe but not visited
                    elif agent_kb['wumpus_possible'][r][c] and agent_kb['pit_possible'][r][c]:
                        cell_content = '?'  # Both dangers possible
                    elif agent_kb['wumpus_possible'][r][c]:
                        cell_content = 'w'  # Possible Wumpus
                    elif agent_kb['pit_possible'][r][c]:
                        cell_content = 'p'  # Possible pit
                
                line += f" {cell_content} |"
            print(line)
            print("-" * (self.size * 4 + 1))
        
        # Print status info
        print(f"Arrow: {'Yes' if self.has_arrow else 'No'} | Gold: {'Yes' if self.has_gold else 'No'} | Score: {self.score}")


class Agent:
    """Online search agent for the Wumpus World"""
    
    def __init__(self, world_size):
        self.world_size = world_size
        self.knowledge_base = {
            'visited': [[False for _ in range(world_size)] for _ in range(world_size)],
            'safe': [[False for _ in range(world_size)] for _ in range(world_size)],
            'wumpus_possible': [[False for _ in range(world_size)] for _ in range(world_size)],
            'pit_possible': [[False for _ in range(world_size)] for _ in range(world_size)],
            'path': []  # Path history for backtracking
        }
        self.current_pos = (0, 0)
        self.current_dir = 'East'
        self.has_arrow = True
        self.has_gold = False
        self.plan = []  # Current action plan
        self.frontier = []  # Frontier for exploration
        
        # Mark starting position as safe and visited
        self.knowledge_base['safe'][0][0] = True
        self.knowledge_base['visited'][0][0] = True

    def update_knowledge_base(self, percepts):
        """Update knowledge base based on current percepts"""
        r, c = self.current_pos
        self.knowledge_base['visited'][r][c] = True
        self.knowledge_base['safe'][r][c] = True
        
        # Get neighboring cells
        neighbors = self._get_neighbors(r, c)
        
        # If no breeze, all neighbors are pit-free
        if 'Breeze' not in percepts:
            for nr, nc in neighbors:
                if self._is_valid(nr, nc):
                    self.knowledge_base['pit_possible'][nr][nc] = False
                    # If cell is not Wumpus possible either, mark it safe
                    if not self.knowledge_base['wumpus_possible'][nr][nc]:
                        self.knowledge_base['safe'][nr][nc] = True
        else:
            # If breeze, neighbors might have pits
            for nr, nc in neighbors:
                if self._is_valid(nr, nc) and not self.knowledge_base['visited'][nr][nc]:
                    self.knowledge_base['pit_possible'][nr][nc] = True
        
        # If no stench, all neighbors are Wumpus-free
        if 'Stench' not in percepts:
            for nr, nc in neighbors:
                if self._is_valid(nr, nc):
                    self.knowledge_base['wumpus_possible'][nr][nc] = False
                    # If cell is not pit possible either, mark it safe
                    if not self.knowledge_base['pit_possible'][nr][nc]:
                        self.knowledge_base['safe'][nr][nc] = True
        else:
            # If stench, neighbors might have Wumpus
            for nr, nc in neighbors:
                if self._is_valid(nr, nc) and not self.knowledge_base['visited'][nr][nc]:
                    self.knowledge_base['wumpus_possible'][nr][nc] = True
        
        # If glitter, plan to grab gold
        if 'Glitter' in percepts:
            self.plan = ['Grab']
        
        # Record current position in path history
        if not self.knowledge_base['path'] or self.knowledge_base['path'][-1] != self.current_pos:
            self.knowledge_base['path'].append(self.current_pos)
        
        # Update frontier (safe but unvisited cells for exploration)
        self._update_frontier()

    def _update_frontier(self):
        """Update the frontier of exploration (safe but unvisited cells)"""
        self.frontier = []
        for r in range(self.world_size):
            for c in range(self.world_size):
                if self.knowledge_base['safe'][r][c] and not self.knowledge_base['visited'][r][c]:
                    # Add to frontier with manhattan distance as priority
                    distance = abs(r - self.current_pos[0]) + abs(c - self.current_pos[1])
                    self.frontier.append(((r, c), distance))
        
        # Sort frontier by distance (closest first)
        self.frontier.sort(key=lambda x: x[1])

    def choose_action(self):
        """Choose next action using online search"""
        # If there's a plan, execute it
        if self.plan:
            return self.plan.pop(0)
        
        # If has gold and at start, climb out
        if self.has_gold and self.current_pos == (0, 0):
            return 'Climb'
        
        # If has gold, try to return to start
        if self.has_gold:
            action = self._plan_path_to((0, 0))
            if action:
                return action
        
        # Look for safe unvisited cells adjacent to current position
        r, c = self.current_pos
        neighbors = self._get_neighbors(r, c)
        safe_unvisited = []
        
        for nr, nc in neighbors:
            if self._is_valid(nr, nc) and self.knowledge_base['safe'][nr][nc] and not self.knowledge_base['visited'][nr][nc]:
                safe_unvisited.append((nr, nc))
        
        # If there are safe unvisited neighbors, move to one
        if safe_unvisited:
            target = random.choice(safe_unvisited)
            return self._get_action_to_move(target)
        
        # If frontier exists, plan path to closest frontier cell
        if self.frontier:
            target, _ = self.frontier[0]
            action = self._plan_path_to(target)
            if action:
                return action
        
        # If no frontier but we have arrow, consider shooting at Wumpus
        if self.has_arrow:
            for r in range(self.world_size):
                for c in range(self.world_size):
                    if self.knowledge_base['wumpus_possible'][r][c] and not self.knowledge_base['visited'][r][c]:
                        # Check if Wumpus is in same row or column
                        cr, cc = self.current_pos
                        if r == cr or c == cc:
                            # Try to face Wumpus
                            if r > cr and self.current_dir != 'North':
                                return 'TurnRight' if self.current_dir == 'East' else 'TurnLeft'
                            elif r < cr and self.current_dir != 'South':
                                return 'TurnRight' if self.current_dir == 'West' else 'TurnLeft'
                            elif c > cc and self.current_dir != 'East':
                                return 'TurnRight' if self.current_dir == 'North' else 'TurnLeft'
                            elif c < cc and self.current_dir != 'West':
                                return 'TurnRight' if self.current_dir == 'South' else 'TurnLeft'
                            else:
                                return 'Shoot'
        
        # If no clear safe moves, consider backtracking
        if len(self.knowledge_base['path']) > 1:
            # Get previous position that's not the current one
            prev_pos = None
            for pos in reversed(self.knowledge_base['path'][:-1]):
                if pos != self.current_pos:
                    prev_pos = pos
                    break
            
            if prev_pos:
                # Try to move back to previous position
                return self._get_action_to_move(prev_pos)
        
        # As a last resort, take a risk - explore a cell that might be safe
        risky_moves = []
        for nr, nc in self._get_neighbors(*self.current_pos):
            if self._is_valid(nr, nc) and not self.knowledge_base['visited'][nr][nc]:
                # Calculate risk (simplistic model)
                risk = 0
                if self.knowledge_base['pit_possible'][nr][nc]:
                    risk += 1
                if self.knowledge_base['wumpus_possible'][nr][nc]:
                    risk += 1
                risky_moves.append(((nr, nc), risk))
        
        if risky_moves:
            # Sort by risk (lower first)
            risky_moves.sort(key=lambda x: x[1])
            target, _ = risky_moves[0]
            return self._get_action_to_move(target)
        
        # If all else fails, turn randomly
        return random.choice(['TurnLeft', 'TurnRight'])

    def _get_neighbors(self, r, c):
        """Get the four adjacent cells"""
        return [(r+1, c), (r-1, c), (r, c+1), (r, c-1)]

    def _is_valid(self, r, c):
        """Check if position is within grid bounds"""
        return 0 <= r < self.world_size and 0 <= c < self.world_size

    def _get_action_to_move(self, target):
        """Get action to move towards target position"""
        tr, tc = target
        r, c = self.current_pos
        
        # Target is north
        if tr > r and tc == c:
            if self.current_dir == 'North':
                return 'Forward'
            elif self.current_dir == 'East':
                return 'TurnLeft'
            elif self.current_dir == 'South':
                return 'TurnRight'  # Need to turn twice
            else:  # West
                return 'TurnRight'
        
        # Target is south
        elif tr < r and tc == c:
            if self.current_dir == 'South':
                return 'Forward'
            elif self.current_dir == 'East':
                return 'TurnRight'
            elif self.current_dir == 'North':
                return 'TurnRight'  # Need to turn twice
            else:  # West
                return 'TurnLeft'
        
        # Target is east
        elif tr == r and tc > c:
            if self.current_dir == 'East':
                return 'Forward'
            elif self.current_dir == 'North':
                return 'TurnRight'
            elif self.current_dir == 'South':
                return 'TurnLeft'
            else:  # West
                return 'TurnRight'  # Need to turn twice
        
        # Target is west
        elif tr == r and tc < c:
            if self.current_dir == 'West':
                return 'Forward'
            elif self.current_dir == 'North':
                return 'TurnLeft'
            elif self.current_dir == 'South':
                return 'TurnRight'
            else:  # East
                return 'TurnRight'  # Need to turn twice
        
        return 'TurnLeft'  # Default action

    def _find_path_to(self, target):
        """Find path from current position to target using BFS"""
        start = self.current_pos
        queue = deque([(start, [])])  # (position, path)
        visited = set([start])
        
        while queue:
            (r, c), path = queue.popleft()
            
            if (r, c) == target:
                return path
            
            for nr, nc in self._get_neighbors(r, c):
                if self._is_valid(nr, nc) and (nr, nc) not in visited and self.knowledge_base['safe'][nr][nc]:
                    visited.add((nr, nc))
                    queue.append(((nr, nc), path + [(nr, nc)]))
        
        return []  # No path found

    def _plan_path_to(self, target):
        """Plan path to target position and return first action"""
        # Create a simplified graph of safe cells
        graph = {}
        for r in range(self.world_size):
            for c in range(self.world_size):
                if self.knowledge_base['safe'][r][c]:
                    neighbors = []
                    for nr, nc in self._get_neighbors(r, c):
                        if self._is_valid(nr, nc) and self.knowledge_base['safe'][nr][nc]:
                            neighbors.append((nr, nc))
                    graph[(r, c)] = neighbors
        
        # Run BFS to find path
        start = self.current_pos
        if start not in graph or target not in graph:
            return None
        
        queue = deque([(start, [])])
        visited = {start}
        
        while queue:
            node, path = queue.popleft()
            
            if node == target:
                if path:
                    next_pos = path[0]
                    return self._get_action_to_move(next_pos)
                return None
            
            for neighbor in graph.get(node, []):
                if neighbor not in visited:
                    visited.add(neighbor)
                    queue.append((neighbor, path + [neighbor]))
        
        return None  # No path found

    def set_agent_state(self, pos, direction, has_arrow, has_gold):
        """Update agent's internal state"""
        self.current_pos = pos
        self.current_dir = direction
        self.has_arrow = has_arrow
        self.has_gold = has_gold


def clear_screen():
    """Clear console screen"""
    os.system('cls' if os.name == 'nt' else 'clear')


def wait_for_space():
    """等待用户按下空格键"""
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