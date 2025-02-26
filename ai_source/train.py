import neat
import json
import os
import numpy as np
import cv2
import mss
import pyautogui
import time
import subprocess
import multiprocessing as mp
import pickle
import psutil
import ctypes
import sys
import platform
import win32gui
import win32con
import win32api
import keyboard
import logging
from collections import deque
from datetime import datetime

# Set up logging
log_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "logs")
os.makedirs(log_dir, exist_ok=True)
log_file = os.path.join(log_dir, f"training_{datetime.now().strftime('%Y%m%d_%H%M%S')}.log")

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler(log_file),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger("OpenGD_AI")

# Platform-specific imports
IS_WINDOWS = platform.system() == 'Windows'
if IS_WINDOWS:
    import win32gui
    import win32con
    import win32api
    logger.info("Running on Windows platform")
else:
    logger.info(f"Running on {platform.system()} platform")

# Load config file
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CONFIG_PATH = os.path.join(SCRIPT_DIR, "config.json")

try:
    with open(CONFIG_PATH, "r") as f:
        config = json.load(f)
    logger.info(f"Loaded configuration from {CONFIG_PATH}")
except Exception as e:
    logger.error(f"Failed to load config file: {e}")
    raise

# Get platform-specific game path
platform_name = platform.system().lower()
if isinstance(config["game_path"], dict):
    GAME_PATH = os.path.abspath(os.path.join(SCRIPT_DIR, "..", config["game_path"].get(platform_name, "")))
else:
    GAME_PATH = os.path.abspath(os.path.join(SCRIPT_DIR, "..", config["game_path"]))
logger.info(f"Game path: {GAME_PATH}")

# Define the position file path based on platform
if platform_name == 'windows':
    app_data = os.getenv('LOCALAPPDATA')
    POSITION_FILE = os.path.join(app_data, "OpenGD", "player_position.txt")
else:  # macOS/Linux
    home = os.path.expanduser("~")
    POSITION_FILE = os.path.join(home, ".OpenGD", "player_position.txt")
logger.info(f"Position file path: {POSITION_FILE}")

# Constants
MAX_GENERATIONS = 100
TIMEOUT_SECONDS = 30  # Maximum time for each attempt
logger.info(f"Training parameters: MAX_GENERATIONS={MAX_GENERATIONS}, TIMEOUT_SECONDS={TIMEOUT_SECONDS}")

# Store history of player positions for temporal context
position_history = deque(maxlen=5)  # Store last 5 positions

# Create a directory to store detailed genome data
genome_data_dir = os.path.join(log_dir, "genome_data")
os.makedirs(genome_data_dir, exist_ok=True)

def get_player_position():
    """Read player position from the file written by the game"""
    try:
        # Check if file exists
        if not os.path.exists(POSITION_FILE):
            return None
            
        # Read the file
        with open(POSITION_FILE, 'r') as f:
            data = f.read().strip()
            
        # Parse the data
        if data:
            parts = data.split(',')
            if len(parts) >= 5:
                x, y, y_vel, on_ground, is_dead = parts[:5]
                return {
                    'x': float(x),
                    'y': float(y),
                    'y_vel': float(y_vel),
                    'on_ground': bool(int(on_ground)),
                    'is_dead': bool(int(is_dead))
                }
        return None
    except Exception as e:
        logger.error(f"Error reading player position: {e}")
        return None

def update_position_history(position):
    """Add new position to history"""
    if position is not None:
        position_history.append(position)

def prepare_inputs():
    """Convert position history to neural network inputs"""
    if not position_history:
        return None
        
    # Create input array
    inputs = []
    
    # Add current position data
    current = position_history[-1]
    inputs.extend([
        current['x'] / 1000.0,  # Normalize x position
        current['y'] / 500.0,   # Normalize y position
        current['y_vel'] / 20.0,  # Normalize y velocity
        1.0 if current['on_ground'] else 0.0
    ])
    
    # Add historical data (differences from current position)
    for i in range(len(position_history) - 2, -1, -1):
        if i >= 0:
            prev = position_history[i]
            inputs.extend([
                (current['x'] - prev['x']) / 50.0,  # Normalized x difference
                (current['y'] - prev['y']) / 50.0,  # Normalized y difference
                (current['y_vel'] - prev['y_vel']) / 10.0  # Normalized velocity difference
            ])
        else:
            # Padding if not enough history
            inputs.extend([0.0, 0.0, 0.0])
    
    return inputs

def start_game():
    """Start the OpenGD game"""
    logger.info(f"Starting OpenGD from {GAME_PATH}...")
    try:
        process = subprocess.Popen(GAME_PATH, shell=True)
        time.sleep(5)  # Wait for game to start
        logger.info("Game started successfully")
        return process
    except Exception as e:
        logger.error(f"Failed to start game: {e}")
        return None

def reset_game():
    """Reset the game by pressing R"""
    logger.debug("Resetting game by pressing 'R'")
    try:
        keyboard.press_and_release('r')
        time.sleep(1)  # Wait for reset
        logger.debug("Game reset completed")
    except Exception as e:
        logger.error(f"Failed to reset game: {e}")

def log_decision(inputs, output, should_jump, position):
    """Log neural network decision details for debugging"""
    if logger.isEnabledFor(logging.DEBUG):
        logger.debug(f"Position: x={position['x']:.2f}, y={position['y']:.2f}, y_vel={position['y_vel']:.2f}, on_ground={position['on_ground']}")
        logger.debug(f"Network inputs: {inputs}")
        logger.debug(f"Network output: {output[0]:.4f}, Decision: {'JUMP' if should_jump else 'NO JUMP'}")

def evaluate_genome(genome, config):
    """Evaluate a single genome"""
    genome_id = genome.key
    logger.info(f"Evaluating genome {genome_id}")
    
    # Create neural network from genome
    net = neat.nn.FeedForwardNetwork.create(genome, config)
    
    # Start game
    game_process = start_game()
    if game_process is None:
        logger.error("Failed to start game, skipping genome evaluation")
        return 0
    
    # Clear position history
    position_history.clear()
    
    # Track fitness metrics
    max_x = 0
    start_time = time.time()
    jumps = 0
    last_jump_time = 0
    
    # Create a log for this genome's run
    genome_log = []
    
    try:
        # Main game loop
        while True:
            # Check for timeout
            current_time = time.time()
            elapsed = current_time - start_time
            if elapsed > TIMEOUT_SECONDS:
                logger.info(f"Timeout reached for genome {genome_id} after {elapsed:.2f} seconds")
                break
                
            # Get player position
            position = get_player_position()
            if position is None:
                time.sleep(0.05)
                continue
                
            # Update history
            update_position_history(position)
            
            # Check if player is dead
            if position['is_dead']:
                logger.info(f"Player died at x={position['x']:.2f}, y={position['y']:.2f}")
                break
                
            # Update max_x for fitness
            if position['x'] > max_x:
                max_x = position['x']
                
            # Prepare inputs for neural network
            inputs = prepare_inputs()
            if inputs is None or len(position_history) < 2:  # Need at least 2 positions for meaningful input
                continue
                
            # Get neural network output
            output = net.activate(inputs)
            
            # Decide whether to jump
            should_jump = output[0] > 0.5
            
            # Log decision details
            log_decision(inputs, output, should_jump, position)
            
            # Record this frame's data
            genome_log.append({
                'time': elapsed,
                'x': position['x'],
                'y': position['y'],
                'y_vel': position['y_vel'],
                'on_ground': position['on_ground'],
                'output': output[0],
                'jumped': False
            })
            
            # Execute jump if needed (with cooldown to prevent spam)
            if should_jump and current_time - last_jump_time > 0.2:
                keyboard.press_and_release('space')
                jumps += 1
                last_jump_time = current_time
                logger.debug(f"Jump executed at x={position['x']:.2f}, y={position['y']:.2f}")
                # Mark this jump in the log
                genome_log[-1]['jumped'] = True
                
            time.sleep(0.05)  # Small delay to prevent CPU overuse
            
    except Exception as e:
        logger.error(f"Error during evaluation of genome {genome_id}: {e}")
    finally:
        # Clean up
        try:
            game_process.terminate()
            logger.info("Game process terminated")
            time.sleep(1)
        except Exception as e:
            logger.error(f"Error terminating game process: {e}")
    
    # Calculate fitness based on distance traveled and survival
    fitness = max_x
    
    # Penalize excessive jumping
    jump_penalty = 0
    if jumps > max_x / 50:  # Arbitrary threshold
        jump_penalty = (jumps - max_x / 50) * 10
        fitness -= jump_penalty
    
    # Log evaluation results
    logger.info(f"Genome {genome_id} evaluation complete:")
    logger.info(f"  - Max distance: {max_x:.2f}")
    logger.info(f"  - Jumps: {jumps}")
    logger.info(f"  - Jump penalty: {jump_penalty:.2f}")
    logger.info(f"  - Final fitness: {fitness:.2f}")
    
    # Save detailed genome run data
    try:
        genome_file = os.path.join(genome_data_dir, f"genome_{genome_id}_{int(fitness)}.json")
        with open(genome_file, 'w') as f:
            json.dump({
                'genome_id': genome_id,
                'fitness': fitness,
                'max_x': max_x,
                'jumps': jumps,
                'frames': genome_log
            }, f, indent=2)
        logger.debug(f"Saved detailed genome data to {genome_file}")
    except Exception as e:
        logger.error(f"Failed to save genome data: {e}")
    
    return fitness

def run_neat():
    """Run the NEAT algorithm to train the AI"""
    logger.info("Starting NEAT training process")
    
    # Load NEAT configuration
    config_path = os.path.join(SCRIPT_DIR, "config-feedforward.txt")
    try:
        config = neat.Config(
            neat.DefaultGenome,
            neat.DefaultReproduction,
            neat.DefaultSpeciesSet,
            neat.DefaultStagnation,
            config_path
        )
        logger.info(f"Loaded NEAT configuration from {config_path}")
    except Exception as e:
        logger.error(f"Failed to load NEAT configuration: {e}")
        raise
    
    # Create population
    population = neat.Population(config)
    
    # Add reporters to show progress
    population.add_reporter(neat.StdOutReporter(True))
    stats = neat.StatisticsReporter()
    population.add_reporter(stats)
    
    # Add a checkpoint reporter to save progress
    checkpoint_dir = os.path.join(log_dir, "checkpoints")
    os.makedirs(checkpoint_dir, exist_ok=True)
    population.add_reporter(neat.Checkpointer(
        generation_interval=5,
        time_interval_seconds=300,
        filename_prefix=os.path.join(checkpoint_dir, "neat-checkpoint-")
    ))
    
    logger.info(f"Starting evolution for up to {MAX_GENERATIONS} generations")
    
    # Run for up to MAX_GENERATIONS generations
    try:
        winner = population.run(evaluate_genome, MAX_GENERATIONS)
        
        # Save the winner
        winner_file = os.path.join(log_dir, "best_ai.pkl")
        with open(winner_file, "wb") as f:
            pickle.dump(winner, f)
        
        # Also save a copy in the main directory
        with open("best_ai.pkl", "wb") as f:
            pickle.dump(winner, f)
            
        logger.info(f"Training complete! Best genome saved to {winner_file}")
        
        # Log winner stats
        logger.info("Best genome details:")
        logger.info(f"  - Key: {winner.key}")
        logger.info(f"  - Fitness: {winner.fitness}")
        logger.info(f"  - Nodes: {len(winner.nodes)}")
        logger.info(f"  - Connections: {len(winner.connections)}")
        
        return winner
    except Exception as e:
        logger.error(f"Error during NEAT evolution: {e}")
        raise

if __name__ == "__main__":
    logger.info("=" * 50)
    logger.info("STARTING OPENGD AI TRAINING")
    logger.info("=" * 50)
    
    try:
        # Check if config-feedforward.txt exists
        config_path = os.path.join(SCRIPT_DIR, "config-feedforward.txt")
        if not os.path.exists(config_path):
            logger.error(f"NEAT configuration file not found: {config_path}")
            raise FileNotFoundError(f"NEAT configuration file not found: {config_path}")
        
        # Run the NEAT algorithm
        run_neat()
    except Exception as e:
        logger.critical(f"Training failed with error: {e}", exc_info=True)
    finally:
        logger.info("=" * 50)
        logger.info("TRAINING PROCESS ENDED")
        logger.info("=" * 50)
