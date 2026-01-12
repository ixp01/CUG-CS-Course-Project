import random
import copy
import tkinter as tk
from tkinter import messagebox

class TicTacToeGUI:
    def __init__(self):
        self.window = tk.Tk()
        self.window.title("井字棋")
        self.window.configure(bg='#f0f0f0')
        
        # 初始化游戏状态
        self.board = [[' ' for _ in range(3)] for _ in range(3)]
        
        # 随机分配玩家角色 (X 或 O)
        self.players = ['X', 'O']
        random.shuffle(self.players)
        self.human_player = self.players[0]
        self.ai_player = self.players[1]
        
        # 随机决定谁先手 - 这里是关键修改
        first_player = random.choice([self.human_player, self.ai_player])
        self.current_turn = first_player
        
        # 创建状态标签
        starter = "你" if first_player == self.human_player else "AI"
        self.status_label = tk.Label(
            self.window,
            text=f"你是 {self.human_player}，AI 是 {self.ai_player}。{starter}先手！",
            font=('Arial', 12),
            bg='#f0f0f0'
        )
        self.status_label.grid(row=0, column=0, columnspan=3, pady=10)
        
        # 创建按钮网格
        self.buttons = []
        for i in range(3):
            row = []
            for j in range(3):
                button = tk.Button(
                    self.window,
                    text='',
                    font=('Arial', 20, 'bold'),
                    width=5,
                    height=2,
                    command=lambda r=i, c=j: self.make_move(r, c)
                )
                button.grid(row=i+1, column=j, padx=5, pady=5)
                row.append(button)
            self.buttons.append(row)
        
        # 创建重新开始按钮
        restart_button = tk.Button(
            self.window,
            text="重新开始",
            font=('Arial', 12),
            command=self.restart_game
        )
        restart_button.grid(row=4, column=0, columnspan=3, pady=10)
        
        # 如果AI先手，立即行动
        if self.current_turn == self.ai_player:
            self.window.after(500, self.ai_move)  # 延迟500ms，让界面先显示

    def make_move(self, row, col):
        if self.board[row][col] == ' ' and self.current_turn == self.human_player:
            # 人类玩家移动
            self.board[row][col] = self.human_player
            self.buttons[row][col].config(
                text=self.human_player,
                fg='red' if self.human_player == 'X' else 'green'
            )
            
            if self.check_game_over():
                return
                
            self.current_turn = self.ai_player
            self.window.after(500, self.ai_move)  # 延迟AI移动，提升用户体验

    def ai_move(self):
        if self.current_turn == self.ai_player:
            # AI移动
            best_move_row, best_move_col = self.find_best_move(self.board)
            self.board[best_move_row][best_move_col] = self.ai_player
            self.buttons[best_move_row][best_move_col].config(
                text=self.ai_player,
                fg='red' if self.ai_player == 'X' else 'green'
            )
            
            if self.check_game_over():
                return
                
            self.current_turn = self.human_player

    def check_game_over(self):
        if self.is_winner(self.board, self.current_turn):
            winner = "你" if self.current_turn == self.human_player else "AI"
            messagebox.showinfo("游戏结束", f"{winner}赢了！")
            return True
        elif self.is_board_full(self.board):
            messagebox.showinfo("游戏结束", "平局！")
            return True
        return False

    def restart_game(self):
        # 重置游戏状态
        self.board = [[' ' for _ in range(3)] for _ in range(3)]
        
        # 重新随机分配玩家角色
        random.shuffle(self.players)
        self.human_player = self.players[0]
        self.ai_player = self.players[1]
        
        # 重新随机决定先手
        first_player = random.choice([self.human_player, self.ai_player])
        self.current_turn = first_player
        
        # 更新状态标签
        starter = "你" if first_player == self.human_player else "AI"
        self.status_label.config(text=f"你是 {self.human_player}，AI 是 {self.ai_player}。{starter}先手！")
        
        # 清空按钮
        for i in range(3):
            for j in range(3):
                self.buttons[i][j].config(text='')
        
        # 如果AI先手，立即行动
        if self.current_turn == self.ai_player:
            self.window.after(500, self.ai_move)

    def is_winner(self, board, player):
        # 检查行
        for row in board:
            if all(cell == player for cell in row):
                return True
        # 检查列
        for col in range(3):
            if all(board[row][col] == player for row in range(3)):
                return True
        # 检查对角线
        if all(board[i][i] == player for i in range(3)):
            return True
        if all(board[i][2-i] == player for i in range(3)):
            return True
        return False

    def is_board_full(self, board):
        return all(cell != ' ' for row in board for cell in row)

    def get_empty_cells(self, board):
        return [(r, c) for r in range(3) for c in range(3) if board[r][c] == ' ']

    def minimax(self, board, depth, is_maximizing_player, alpha, beta):
        if self.is_winner(board, self.ai_player):
            return 1
        if self.is_winner(board, self.human_player):
            return -1
        if self.is_board_full(board):
            return 0

        if is_maximizing_player:
            max_eval = float('-inf')
            for r, c in self.get_empty_cells(board):
                board[r][c] = self.ai_player
                eval = self.minimax(board, depth + 1, False, alpha, beta)
                board[r][c] = ' '
                max_eval = max(max_eval, eval)
                alpha = max(alpha, eval)
                if beta <= alpha:
                    break
            return max_eval
        else:
            min_eval = float('inf')
            for r, c in self.get_empty_cells(board):
                board[r][c] = self.human_player
                eval = self.minimax(board, depth + 1, True, alpha, beta)
                board[r][c] = ' '
                min_eval = min(min_eval, eval)
                beta = min(beta, eval)
                if beta <= alpha:
                    break
            return min_eval

    def find_best_move(self, board):
        best_move = (-1, -1)
        best_eval = float('-inf')
        for r, c in self.get_empty_cells(board):
            board_copy = copy.deepcopy(board)
            board_copy[r][c] = self.ai_player
            eval = self.minimax(board_copy, 0, False, float('-inf'), float('inf'))
            if eval > best_eval:
                best_eval = eval
                best_move = (r, c)
        return best_move

    def run(self):
        self.window.mainloop()

if __name__ == "__main__":
    game = TicTacToeGUI()
    game.run() 