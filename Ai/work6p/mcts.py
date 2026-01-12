import numpy as np
import torch
from collections import defaultdict
import math
import copy
from neural_network import prepare_input # 导入 prepare_input 函数

class Node:
    def __init__(self, prior=0, player=None, move=None):
        self.visit_count = 0
        self.value_sum = 0
        self.prior = prior
        self.children = {}
        self.state_value = 0
        self.player = player  # Store which player's turn it is at this node
        self.move = move      # Store the move that led to this node
        
    def expanded(self):
        return len(self.children) > 0
    
    def value(self):
        if self.visit_count == 0:
            return 0
        return self.value_sum / self.visit_count

class MCYTS:
    def __init__(self, policy_network, value_network, game, num_simulations=400, c_puct=1.0):
        self.policy_network = policy_network
        self.value_network = value_network
        self.game = game
        self.num_simulations = num_simulations
        self.c_puct = c_puct
        
    def search(self, root_state):
        """Run MCTS search and return the best move."""
        # Create root node
        root = Node(player=self.game.get_current_player())
        
        # Get initial policy and value predictions
        state_tensor = prepare_input(root_state, root.player)
        with torch.no_grad():
            policy, value = self.policy_network(state_tensor), self.value_network(state_tensor)
        
        # Initialize root node with policy priors
        valid_moves = self.game.get_valid_moves(self.game.get_current_player())
        for move in valid_moves:
            root.children[move] = Node(
                prior=policy[0, move[0], move[1]].item(), 
                player=3 - root.player,
                move=move
            )
        root.state_value = value.item()
        
        # Run simulations
        for _ in range(self.num_simulations):
            # Always use a fresh copy of the game for each simulation
            game_copy = copy.deepcopy(self.game)
            
            # Run a single simulation
            self._simulate(root, game_copy, root_state.copy())
        
        # Select best move
        return self._select_best_move(root)
    
    def _simulate(self, root, game_copy, current_state):
        """Run a single MCTS simulation."""
        node = root
        search_path = [node]
        
        # Selection - find a leaf node
        while node.expanded() and not game_copy.is_game_over():
            move = self._select_child(node)
            child_node = node.children[move]
            search_path.append(child_node)
            
            # Make the move in our simulation
            game_copy.make_move(move[0], move[1])
            
            # If next player has no valid moves, simulate a skip
            if not game_copy.is_game_over() and not game_copy.has_valid_moves(game_copy.get_current_player()):
                game_copy.skip_turn()
            
            node = child_node
            node.player = game_copy.get_current_player()
        
        # Expansion and evaluation
        if not game_copy.is_game_over():
            current_player = game_copy.get_current_player()
            state_tensor = prepare_input(game_copy.get_board_state(), current_player)
            
            with torch.no_grad():
                policy, value = self.policy_network(state_tensor), self.value_network(state_tensor)
            
            valid_moves = game_copy.get_valid_moves(current_player)
            for move in valid_moves:
                node.children[move] = Node(
                    prior=policy[0, move[0], move[1]].item(), 
                    player=3 - current_player,
                    move=move
                )
            node.state_value = value.item()
        
        # Backpropagation
        self._backpropagate(search_path, game_copy.get_winner())
    
    def _select_child(self, node):
        """Select child node using UCB formula with policy prior."""
        ucb_values = {}
        for move, child in node.children.items():
            # UCB formula with policy prior
            ucb = child.value() + self.c_puct * child.prior * math.sqrt(node.visit_count) / (1 + child.visit_count)
            ucb_values[move] = ucb
        
        return max(ucb_values.items(), key=lambda x: x[1])[0]
    
    def _backpropagate(self, search_path, winner):
        """Backpropagate the value through the search path."""
        for node in reversed(search_path):
            node.visit_count += 1
            
            if winner is not None:
                if winner == 0:  # Draw
                    node.value_sum += 0
                else:
                    # Value is 1 if node player won, -1 if lost
                    node.value_sum += 1 if winner == node.player else -1
            else:
                # If simulation didn't reach a terminal state, use value network prediction
                node.value_sum += node.state_value
    
    def _select_best_move(self, root):
        """Select the move with the highest visit count."""
        if not root.children:
            return None  # No valid moves
            
        visit_counts = {move: child.visit_count for move, child in root.children.items()}
        return max(visit_counts.items(), key=lambda x: x[1])[0]

# Add a helper method to MCYTS for training (This is already defined in train.py and added to MCYTS)
# You should keep this method in train.py as it's specifically for training logic.
# def search_for_training(self, root_state, temperature=1.0):
#     """Search with MCTS and return visit counts for each move."""
#     root = self.search(root_state) # Now self.search returns a Node object
#     visit_counts = {move: child.visit_count for move, child in root.children.items()}
#     # ... (rest of search_for_training) ...

# MCYTS.search_for_training = search_for_training # This line should remain in train.py