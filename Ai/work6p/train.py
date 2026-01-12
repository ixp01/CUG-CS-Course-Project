import os
import torch
import torch.nn as nn
import torch.optim as optim
import numpy as np
from tqdm import tqdm
import random
from collections import deque
import pickle

from reversi import Reversi
from neural_network import PolicyNetwork, ValueNetwork, prepare_input
from mcts import MCYTS

class Trainer:
    def __init__(self, policy_net=None, value_net=None, device="cpu"):
        # Initialize networks
        self.device = device
        self.policy_net = PolicyNetwork().to(device) if policy_net is None else policy_net
        self.value_net = ValueNetwork().to(device) if value_net is None else value_net
        
        # Initialize optimizers
        self.policy_optimizer = optim.Adam(self.policy_net.parameters(), lr=0.001)
        self.value_optimizer = optim.Adam(self.value_net.parameters(), lr=0.001)
        
        # Define loss functions
        self.policy_criterion = nn.CrossEntropyLoss()
        self.value_criterion = nn.MSELoss()
        
        # Initialize game environment
        self.game = Reversi()
        
        # Initialize MCTS
        self.mcts = MCYTS(self.policy_net, self.value_net, self.game, num_simulations=200)
        
        # Replay buffer
        self.buffer_size = 10000
        self.replay_buffer = deque(maxlen=self.buffer_size)
        
    def self_play(self, num_games=100, temperature=1.0):
        """Generate self-play data by playing games against itself."""
        all_examples = []
        
        for game_idx in tqdm(range(num_games), desc="Self-play games"):
            game_examples = []
            self.game.reset_board()
            game_step = 0
            
            while not self.game.is_game_over():
                current_player = self.game.get_current_player()
                board_state = self.game.get_board_state()
                
                # Skip turn if no valid moves
                if not self.game.has_valid_moves(current_player):
                    self.game.skip_turn()
                    continue
                
                # Get MCTS policy
                move_probs = np.zeros((8, 8))
                if temperature == 0:  # Deterministic
                    move = self.mcts.search(board_state)
                    move_probs[move[0], move[1]] = 1.0
                else:  # Sample based on visit counts
                    root = self.mcts.search_for_training(board_state, temperature)
                    for move, count in root.items():
                        move_probs[move[0], move[1]] = count
                    
                    # Normalize
                    if np.sum(move_probs) > 0:
                        move_probs /= np.sum(move_probs)
                    
                    # Choose move based on policy
                    valid_moves = self.game.get_valid_moves(current_player)
                    moves_prob = [(move, move_probs[move[0], move[1]]) for move in valid_moves]
                    moves, probs = zip(*moves_prob)
                    move = moves[np.random.choice(len(moves), p=probs)]
                
                # Store state, policy, and player
                game_examples.append((board_state, move_probs, current_player))
                
                # Make move
                self.game.make_move(move[0], move[1])
                game_step += 1
            
            # Get winner
            winner = self.game.get_winner()
            
            # Assign rewards
            for i, (state, policy, player) in enumerate(game_examples):
                if winner == 0:  # Draw
                    value = 0
                else:
                    value = 1 if player == winner else -1
                
                all_examples.append((state, policy, value))
                
        return all_examples
    
    def train(self, examples, batch_size=128, epochs=10):
        """Train policy and value networks using examples from self-play."""
        # Shuffle examples
        random.shuffle(examples)
        
        for epoch in range(epochs):
            policy_losses = []
            value_losses = []
            
            # Train in batches
            for i in range(0, len(examples), batch_size):
                batch = examples[i:i+batch_size]
                states, policies, values = zip(*batch)
                
                # Prepare inputs and targets
                state_tensors = torch.cat([prepare_input(state, 1) for state in states]).to(self.device)  # Player info will be added in prepare_input
                policy_targets = torch.FloatTensor(np.array(policies)).view(-1, 64).to(self.device)
                value_targets = torch.FloatTensor(np.array(values)).view(-1, 1).to(self.device)
                
                # Forward pass
                policy_outputs = self.policy_net(state_tensors).view(-1, 64)
                value_outputs = self.value_net(state_tensors)
                
                # Calculate losses
                policy_loss = self.policy_criterion(policy_outputs, policy_targets)
                value_loss = self.value_criterion(value_outputs, value_targets)
                
                # Backward pass and optimize policy
                self.policy_optimizer.zero_grad()
                policy_loss.backward()
                self.policy_optimizer.step()
                
                # Backward pass and optimize value
                self.value_optimizer.zero_grad()
                value_loss.backward()
                self.value_optimizer.step()
                
                policy_losses.append(policy_loss.item())
                value_losses.append(value_loss.item())
            
            print(f"Epoch {epoch+1}/{epochs}, Policy Loss: {np.mean(policy_losses):.4f}, Value Loss: {np.mean(value_losses):.4f}")
    
    def save_models(self, path="models"):
        """Save policy and value networks."""
        os.makedirs(path, exist_ok=True)
        torch.save(self.policy_net.state_dict(), os.path.join(path, "policy_net.pth"))
        torch.save(self.value_net.state_dict(), os.path.join(path, "value_net.pth"))
    
    def load_models(self, path="models"):
        """Load policy and value networks."""
        policy_path = os.path.join(path, "policy_net.pth")
        value_path = os.path.join(path, "value_net.pth")
        
        if os.path.exists(policy_path):
            self.policy_net.load_state_dict(torch.load(policy_path, map_location=self.device))
        
        if os.path.exists(value_path):
            self.value_net.load_state_dict(torch.load(value_path, map_location=self.device))
    
    def train_and_save(self, iterations=10, games_per_iteration=10, temperature_schedule=None):
        """Train and save models over multiple iterations."""
        if temperature_schedule is None:
            temperature_schedule = [1.0] * iterations  # Constant temperature
        
        for i in range(iterations):
            print(f"Iteration {i+1}/{iterations}")
            temperature = temperature_schedule[i] if i < len(temperature_schedule) else 0.0
            
            # Generate self-play data
            examples = self.self_play(num_games=games_per_iteration, temperature=temperature)
            
            # Add to replay buffer
            self.replay_buffer.extend(examples)
            
            # Sample from buffer for training
            sample_size = min(len(self.replay_buffer), 1000)
            train_examples = random.sample(list(self.replay_buffer), sample_size)
            
            # Train networks
            self.train(train_examples)
            
            # Save models
            self.save_models()
            
            print(f"Completed iteration {i+1}, buffer size: {len(self.replay_buffer)}")

# Add a helper method to MCYTS for training
def search_for_training(self, root_state, temperature=1.0):
    """Search with MCTS and return visit counts for each move."""
    # Similar to normal search but returns visit counts
    root = self.search(root_state)
    visit_counts = {move: child.visit_count for move, child in root.children.items()}
    
    # Apply temperature
    if temperature != 0:
        counts = np.array(list(visit_counts.values()), dtype=np.float64)
        counts = np.power(counts, 1.0/temperature)
        total = np.sum(counts)
        probs = counts / total if total > 0 else counts
        moves = list(visit_counts.keys())
        visit_counts = {moves[i]: probs[i] for i in range(len(moves))}
    
    return visit_counts

# Add the method to the MCYTS class
MCYTS.search_for_training = search_for_training

if __name__ == "__main__":
    # Create trainer
    trainer = Trainer()
    
    # Define training schedule
    # Start with high temperature (exploration) and gradually decrease (exploitation)
    temperature_schedule = [1.0, 1.0, 0.8, 0.8, 0.5, 0.5, 0.25, 0.25, 0.1, 0.0]
    
    # Train and save models
    trainer.train_and_save(iterations=20, games_per_iteration=10, temperature_schedule=temperature_schedule) 