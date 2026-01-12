import numpy as np

class Reversi:
    def __init__(self):
        self.board_size = 8
        self.reset_board()
        
    def reset_board(self):
        """Initialize the board with the starting position."""
        # 创建一个空棋盘（全部为0）
        self.board = np.zeros((self.board_size, self.board_size), dtype=int)
        
        # 标准反转棋初始布局 - 中心的2x2放置黑白棋子交错排列
        mid = self.board_size // 2
        self.board[mid-1, mid-1] = 2  # 左上白
        self.board[mid-1, mid] = 1    # 右上黑
        self.board[mid, mid-1] = 1    # 左下黑
        self.board[mid, mid] = 2      # 右下白
        
        self.current_player = 1  # 1 for black, 2 for white
        
    def get_valid_moves(self, player):
        """Get all valid moves for the current player."""
        valid_moves = []
        for i in range(self.board_size):
            for j in range(self.board_size):
                if self.is_valid_move(i, j, player):
                    valid_moves.append((i, j))
        return valid_moves
    
    def has_valid_moves(self, player):
        """Check if the player has any valid moves."""
        return len(self.get_valid_moves(player)) > 0
    
    def skip_turn(self):
        """Skip the current player's turn."""
        self.current_player = 3 - self.current_player
        return True
    
    def is_valid_move(self, row, col, player):
        """Check if a move is valid."""
        # 检查是否超出棋盘范围
        if not (0 <= row < self.board_size and 0 <= col < self.board_size):
            return False
            
        # 检查位置是否已经有棋子
        if self.board[row, col] != 0:
            return False
            
        opponent = 3 - player
        directions = [(0, 1), (1, 0), (0, -1), (-1, 0),
                     (1, 1), (-1, -1), (1, -1), (-1, 1)]
        
        for dx, dy in directions:
            if self._would_flip(row, col, dx, dy, player):
                return True
        return False
    
    def _would_flip(self, row, col, dx, dy, player):
        """Check if a move would flip any pieces in a given direction."""
        opponent = 3 - player
        x, y = row + dx, col + dy
        
        # 检查是否超出棋盘范围或第一个相邻棋子不是对手的
        if not (0 <= x < self.board_size and 0 <= y < self.board_size):
            return False
        if self.board[x, y] != opponent:
            return False
            
        # 沿方向继续检查
        x, y = x + dx, y + dy
        while 0 <= x < self.board_size and 0 <= y < self.board_size:
            if self.board[x, y] == 0:  # 遇到空位置，不能翻转
                return False
            if self.board[x, y] == player:  # 遇到自己的棋子，可以翻转
                return True
            # 继续沿方向检查
            x, y = x + dx, y + dy
            
        return False  # 到达棋盘边缘，没有找到自己的棋子
    
    def make_move(self, row, col):
        """Make a move and flip the appropriate pieces."""
        if not self.is_valid_move(row, col, self.current_player):
            return False
            
        self.board[row, col] = self.current_player
        opponent = 3 - self.current_player
        directions = [(0, 1), (1, 0), (0, -1), (-1, 0),
                     (1, 1), (-1, -1), (1, -1), (-1, 1)]
        
        for dx, dy in directions:
            if self._would_flip(row, col, dx, dy, self.current_player):
                x, y = row + dx, col + dy
                while self.board[x, y] == opponent:
                    self.board[x, y] = self.current_player
                    x, y = x + dx, y + dy
        
        self.current_player = opponent
        return True
    
    def is_game_over(self):
        """Check if the game is over."""
        # Game is over if neither player has valid moves
        return not (self.has_valid_moves(1) or self.has_valid_moves(2))
    
    def get_winner(self):
        """Get the winner of the game."""
        if not self.is_game_over():
            return None
        black_count = np.sum(self.board == 1)
        white_count = np.sum(self.board == 2)
        if black_count > white_count:
            return 1
        elif white_count > black_count:
            return 2
        else:
            return 0  # Draw
    
    def get_board_state(self):
        """Get the current board state."""
        return self.board.copy()
    
    def get_current_player(self):
        """Get the current player."""
        return self.current_player 