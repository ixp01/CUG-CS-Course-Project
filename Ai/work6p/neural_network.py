import torch
import torch.nn as nn
import torch.nn.functional as F
import numpy as np # 添加 numpy 导入

class PolicyNetwork(nn.Module):
    def __init__(self):
        super(PolicyNetwork, self).__init__()
        # Input: 8x8x3 (board state + current player)
        self.conv1 = nn.Conv2d(3, 32, kernel_size=3, padding=1)
        self.conv2 = nn.Conv2d(32, 64, kernel_size=3, padding=1)
        self.conv3 = nn.Conv2d(64, 128, kernel_size=3, padding=1)
        self.fc1 = nn.Linear(128 * 8 * 8, 512)
        self.fc2 = nn.Linear(512, 64)  # 8x8 output for move probabilities
        
    def forward(self, x):
        x = F.relu(self.conv1(x))
        x = F.relu(self.conv2(x))
        x = F.relu(self.conv3(x))
        x = x.view(-1, 128 * 8 * 8)
        x = F.relu(self.fc1(x))
        x = self.fc2(x)
        # 修正：将softmax应用到所有64个位置上，然后重塑
        return F.softmax(x.view(-1, 64), dim=1).view(-1, 8, 8)

class ValueNetwork(nn.Module):
    def __init__(self):
        super(ValueNetwork, self).__init__()
        # Input: 8x8x3 (board state + current player)
        self.conv1 = nn.Conv2d(3, 32, kernel_size=3, padding=1)
        self.conv2 = nn.Conv2d(32, 64, kernel_size=3, padding=1)
        self.conv3 = nn.Conv2d(64, 128, kernel_size=3, padding=1)
        self.fc1 = nn.Linear(128 * 8 * 8, 512)
        self.fc2 = nn.Linear(512, 1)
        
    def forward(self, x):
        x = F.relu(self.conv1(x))
        x = F.relu(self.conv2(x))
        x = F.relu(self.conv3(x))
        x = x.view(-1, 128 * 8 * 8)
        x = F.relu(self.fc1(x))
        x = torch.tanh(self.fc2(x))  # Output between -1 and 1
        return x

def prepare_input(board_state, current_player):
    """Convert board state and current player to network input format."""
    # Create a 3-channel input: empty spaces, black pieces, white pieces
    input_tensor = torch.zeros(3, 8, 8)
    # 修正：使用 .astype(float) 转换为浮点型 NumPy 数组，然后用 torch.from_numpy 转换
    input_tensor[0] = torch.from_numpy((board_state == 0).astype(float))  # Empty spaces
    input_tensor[1] = torch.from_numpy((board_state == 1).astype(float))  # Black pieces
    input_tensor[2] = torch.from_numpy((board_state == 2).astype(float))  # White pieces
    
    # Add current player information
    if current_player == 1:  # Black's turn
        input_tensor[1] += 0.1
    else:  # White's turn
        input_tensor[2] += 0.1
        
    return input_tensor.unsqueeze(0)  # Add batch dimension