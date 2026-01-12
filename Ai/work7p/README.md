# Wumpus World Solver using Reinforcement Learning

This project implements a solution to the classic Wumpus World problem using reinforcement learning techniques, specifically Q-learning. The agent learns through trial and error to navigate the dangerous Wumpus World, find gold, and return safely to the starting position.

## Problem Description

The Wumpus World is a grid-based environment with the following elements:

- **Agent**: The AI explorer that navigates the world
- **Wumpus**: A monster that eats the agent if they enter the same cell
- **Pits**: Holes that cause the agent to fall to its death
- **Gold**: The treasure the agent aims to collect
- **Start**: The entry point (usually the bottom-left corner)

The agent receives percepts (sensory information) at each cell:
- **Stench**: Indicates a Wumpus is in an adjacent cell
- **Breeze**: Indicates a pit is in an adjacent cell
- **Glitter**: Indicates gold is in the current cell
- **Bump**: Indicates the agent hit a wall
- **Scream**: Indicates the Wumpus was killed by an arrow

The agent can perform the following actions:
- **Forward**: Move one cell in the current direction
- **TurnLeft**: Turn 90 degrees to the left
- **TurnRight**: Turn 90 degrees to the right
- **Shoot**: Fire an arrow in the current direction (the agent has only one arrow)
- **Grab**: Pick up the gold if in the same cell
- **Climb**: Exit the cave (only works at the start position)

## Reinforcement Learning Approach

This implementation uses Q-learning to solve the Wumpus World:

1. **State Representation**: Each state includes:
   - Agent's position (row, column)
   - Agent's direction (North, East, South, West)
   - Whether the agent has an arrow
   - Whether the agent has grabbed the gold
   - Percepts (stench, breeze, glitter, scream, bump)

2. **Action Space**: The six possible actions (Forward, TurnLeft, TurnRight, Shoot, Grab, Climb)

3. **Reward Function**:
   - Winning (grabbing gold and climbing out): +1000
   - Death (by Wumpus or pit): -1000
   - Each step: -1 (to encourage efficiency)
   - Grabbing gold: +100
   - Killing Wumpus: +50
   - Bumping into a wall: -10
   - Invalid climbing: -100

4. **Learning Algorithm**: Q-learning with epsilon-greedy policy for exploration/exploitation balance

## Requirements

- Python 3.6+
- NumPy
- Collections module (included in Python standard library)

## Usage

Run the main script to train the agent and see a demonstration:

```
python wumpus_rl.py
```

The program will:
1. Initialize a Wumpus World environment
2. Train the agent using Q-learning for the specified number of episodes
3. Demonstrate the learned policy in a step-by-step visualization

## Parameters

You can modify the following parameters in the `main()` function:

- **world_size**: Size of the grid (default: 4x4)
- **num_pits**: Number of pits in the world (default: 2)
- **has_wumpus**: Whether to include the Wumpus (default: True)
- **random_seed**: Seed for reproducibility (default: 42)
- **num_episodes**: Number of training episodes (default: 5000)
- **max_steps**: Maximum steps per episode (default: 100)
- **learning_rate**: Alpha value for Q-learning (default: 0.1)
- **discount_factor**: Gamma value for future rewards (default: 0.99)
- **epsilon_start**: Initial exploration rate (default: 1.0)
- **epsilon_min**: Minimum exploration rate (default: 0.01)
- **epsilon_decay_rate**: Rate at which epsilon decreases (default: 0.0005)

## Visualization

During the demonstration phase, the program displays:
- The current state of the world as a grid
- The agent's position and direction
- Current percepts
- Chosen action and resulting reward
- Special events (finding gold, killing Wumpus, etc.)

The visualization includes a legend:
- ^ v < > : Agent (direction shown by symbol)
- W : Wumpus
- P : Pit
- G : Gold
- S : Start position 