import sys
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QPushButton, 
                           QVBoxLayout, QHBoxLayout, QLabel, QMessageBox, QFrame)
from PyQt5.QtCore import Qt, QSize, QTimer, QRect
from PyQt5.QtGui import QPainter, QColor, QPen, QBrush, QFont
from reversi import Reversi
from neural_network import PolicyNetwork, ValueNetwork
from mcts import MCYTS
import torch

class ReversiBoard(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.game = Reversi()
        self.cell_size = 55  # 恢复到合适大小
        self.board_size = self.cell_size * 8
        
        # 设置固定大小
        self.setMinimumSize(QSize(self.board_size, self.board_size))
        self.setMaximumSize(QSize(self.board_size, self.board_size))
        self.setStyleSheet("background-color: #0A6522;")  # 深绿色背景
        
        # 初始化神经网络
        self.policy_network = PolicyNetwork()
        self.value_network = ValueNetwork()
        
        # 加载训练好的模型
        try:
            self.policy_network.load_state_dict(torch.load('models/policy_net.pth'))
            self.value_network.load_state_dict(torch.load('models/value_net.pth'))
            print("成功加载训练模型")
        except Exception as e:
            print(f"加载模型出错: {e}")
            print("使用未训练的随机模型")
        
        # 设置为评估模式
        self.policy_network.eval()
        self.value_network.eval()
        
        # 初始化MCTS
        self.mcts = MCYTS(self.policy_network, self.value_network, self.game, num_simulations=400)
        
        # 游戏状态
        self.human_player = 1  # 玩家执黑先行
        self.ai_player = 2     # AI执白后行
        self.ai_thinking = False
        self.game_active = True
        
        self.update()

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        
        # 绘制棋盘网格
        painter.setPen(QPen(QColor(0, 0, 0), 1))
        for i in range(9):
            # 绘制水平线
            painter.drawLine(0, i * self.cell_size, self.board_size, i * self.cell_size)
            # 绘制垂直线
            painter.drawLine(i * self.cell_size, 0, i * self.cell_size, self.board_size)
        
        # 绘制棋子和高亮
        for row in range(8):
            for col in range(8):
                rect = self.cell_rect(row, col)
                
                # 绘制有效落子位置的高亮
                if (self.game.get_current_player() == self.human_player and 
                    not self.ai_thinking and 
                    self.game.is_valid_move(row, col, self.human_player)):
                    painter.setBrush(QBrush(QColor(255, 255, 0, 80)))  # 半透明黄色
                    painter.setPen(Qt.NoPen)
                    painter.drawEllipse(rect.x() + 10, rect.y() + 10, 
                                       self.cell_size - 20, self.cell_size - 20)  # 调整高亮大小
                
                # 绘制棋子
                piece = self.game.board[row, col]
                if piece != 0:
                    if piece == 1:  # 黑棋
                        painter.setBrush(QBrush(QColor(0, 0, 0)))
                    else:  # 白棋
                        painter.setBrush(QBrush(QColor(255, 255, 255)))
                    
                    # 绘制棋子边框
                    painter.setPen(QPen(QColor(0, 0, 0), 1))
                    # 绘制圆形棋子，调整棋子大小适应小格子
                    painter.drawEllipse(rect.x() + 3, rect.y() + 3, 
                                       self.cell_size - 6, self.cell_size - 6)

    def cell_rect(self, row, col):
        """返回指定行列的格子矩形"""
        if not (0 <= row < 8 and 0 <= col < 8):
            return QRect(0, 0, 10, 10)  # 返回一个小矩形作为默认值
            
        x = col * self.cell_size
        y = row * self.cell_size
        return QRect(x, y, self.cell_size, self.cell_size)
    
    def mousePressEvent(self, event):
        if not self.game_active or self.ai_thinking or self.game.get_current_player() != self.human_player:
            return
            
        col = event.x() // self.cell_size
        row = event.y() // self.cell_size
        
        if 0 <= row < 8 and 0 <= col < 8 and self.game.is_valid_move(row, col, self.human_player):
            self.game.make_move(row, col)
            self.update()
            
            # 检查游戏是否结束
            if self.game.is_game_over():
                self.game_active = False
                self.show_game_result()
                return
                
            # 检查AI回合
            QTimer.singleShot(300, self.check_ai_turn)
    
    def check_ai_turn(self):
        """检查是否轮到AI，处理可能的跳过回合"""
        current_player = self.game.get_current_player()
        
        # 如果当前不是AI，但AI没有有效移动，跳过AI回合
        if current_player == self.ai_player and not self.game.has_valid_moves(self.ai_player):
            print("AI没有有效移动，跳过回合")
            self.game.skip_turn()
            self.update()
            
            # 再次检查游戏是否结束
            if self.game.is_game_over():
                self.game_active = False
                self.show_game_result()
                return
                
            # 如果人类玩家也没有有效移动，结束游戏
            if not self.game.has_valid_moves(self.human_player):
                self.game_active = False
                self.show_game_result()
                return
                
            return  # 返回到人类玩家回合
        
        # 如果轮到AI并且有有效移动，进行AI移动
        if current_player == self.ai_player:
            self.make_ai_move()
    
    def make_ai_move(self):
        """让AI进行移动"""
        self.ai_thinking = True
        self.update()
        
        # 使用QTimer允许界面更新
        QTimer.singleShot(500, self.process_ai_move)
    
    def process_ai_move(self):
        """处理AI的移动（在短暂延迟后调用，允许界面更新）"""
        # 使用MCYTS获取AI移动
        move = self.mcts.search(self.game.get_board_state())
        
        # 执行移动
        if move and len(move) == 2 and self.game.is_valid_move(move[0], move[1], self.ai_player):
            self.game.make_move(move[0], move[1])
        else:
            print("AI无法找到有效移动，跳过回合")
            self.game.skip_turn()
        
        self.ai_thinking = False
        self.update()
        
        # 检查游戏是否结束
        if self.game.is_game_over():
            self.game_active = False
            self.show_game_result()
            return
            
        # 检查人类玩家是否有有效移动
        if not self.game.has_valid_moves(self.human_player):
            print("人类玩家没有有效移动，跳过回合")
            self.game.skip_turn()
            
            # 如果AI有有效移动，继续AI回合
            if self.game.get_current_player() == self.ai_player and self.game.has_valid_moves(self.ai_player):
                QTimer.singleShot(500, self.make_ai_move)
            elif self.game.is_game_over():
                self.game_active = False
                self.show_game_result()
    
    def show_game_result(self):
        """显示游戏结果"""
        winner = self.game.get_winner()
        
        if winner == 0:
            result = "游戏结束 - 平局！"
        else:
            result = f"游戏结束 - {'黑方' if winner == 1 else '白方'}胜利！"
            
        black_count = sum(sum(self.game.board == 1))
        white_count = sum(sum(self.game.board == 2))
        score_text = f"得分: 黑方 {black_count} - {white_count} 白方"
        
        msg_box = QMessageBox()
        msg_box.setWindowTitle("游戏结束")
        msg_box.setText(f"{result}\n{score_text}")
        msg_box.setStandardButtons(QMessageBox.Ok)
        msg_box.exec_()
    
    def reset_game(self):
        """重置游戏"""
        self.game.reset_board()
        self.ai_thinking = False
        self.game_active = True
        self.update()


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("反转棋 - MiniAlphaGo (小版本)")
        
        # 创建中央部件和布局
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        main_layout.setSpacing(8)  # 间距稍微小一点
        main_layout.setContentsMargins(8, 8, 8, 8)  # 边距小一点
        
        # 创建游戏面板
        self.board = ReversiBoard()
        main_layout.addWidget(self.board, 1)
        
        # 创建合并的状态栏和按钮面板
        control_panel = QFrame()
        control_panel.setFrameShape(QFrame.StyledPanel)
        control_panel.setStyleSheet("background-color: #E8E8E8; padding: 5px;")
        control_layout = QHBoxLayout(control_panel)
        
        # 创建状态标签（左侧）
        self.status_label = QLabel("轮到黑方（玩家）")
        self.status_label.setFont(QFont("Arial", 10, QFont.Bold))
        self.status_label.setAlignment(Qt.AlignLeft)
        control_layout.addWidget(self.status_label)
        
        # 创建黑白计数标签（中间）
        self.count_label = QLabel("黑: 2 | 白: 2")
        self.count_label.setFont(QFont("Arial", 10))
        self.count_label.setAlignment(Qt.AlignCenter)
        control_layout.addWidget(self.count_label)
        
        # 创建新游戏按钮（右侧）
        new_game_btn = QPushButton("新游戏")
        new_game_btn.setFont(QFont("Arial", 10))
        new_game_btn.setStyleSheet("padding: 3px 10px;")
        new_game_btn.clicked.connect(self.new_game)
        control_layout.addWidget(new_game_btn)
        
        main_layout.addWidget(control_panel)
        
        # 创建状态更新定时器
        self.update_timer = QTimer(self)
        self.update_timer.timeout.connect(self.update_status)
        self.update_timer.start(100)
        
        # 设置窗口大小 - 调整为更小的尺寸
        self.setFixedSize(self.board.board_size + 16, self.board.board_size + 80)
    
    def update_status(self):
        # 计算黑白棋子数量
        black_count = sum(sum(self.board.game.board == 1))
        white_count = sum(sum(self.board.game.board == 2))
        self.count_label.setText(f"黑: {black_count} | 白: {white_count}")
        
        # 更新状态文本，AI思考状态改为显示在窗口标题栏
        if self.board.game.is_game_over():
            self.status_label.setText("游戏结束")
            self.setWindowTitle("反转棋 - MiniAlphaGo (小版本)")
        elif self.board.ai_thinking:
            # AI思考状态不在状态栏显示，改为显示在窗口标题
            current_player = self.board.game.get_current_player()
            if current_player == self.board.human_player:
                self.status_label.setText("轮到黑方（玩家）")
            else:
                self.status_label.setText("轮到白方（AI）")
            self.status_label.setStyleSheet("color: black; font-weight: bold;")
            
            # 在窗口标题显示AI思考状态
            self.setWindowTitle("反转棋 - MiniAlphaGo (AI正在思考中...)")
        else:
            # 恢复正常状态显示和窗口标题
            self.status_label.setStyleSheet("color: black; font-weight: bold;")
            current_player = self.board.game.get_current_player()
            if current_player == self.board.human_player:
                self.status_label.setText("轮到黑方（玩家）")
            else:
                self.status_label.setText("轮到白方（AI）")
            
            # 恢复正常窗口标题
            self.setWindowTitle("反转棋 - MiniAlphaGo (小版本)")

    def new_game(self):
        self.board.reset_game()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_()) 