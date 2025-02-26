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
    """Convert position history to neural network inputs - simplified to just use x position"""
    if not position_history or len(position_history) < 2:
        return None
        
    # Create input array
    inputs = []
    
    # Get current and previous positions
    current = position_history[-1]
    prev = position_history[-2]
    
    # Add the 7 required inputs, but only use x position meaningfully
    inputs = [
        current['x'] / 1000.0,  # Normalized x position
        0.0,  # Placeholder
        0.0,  # Placeholder
        0.0,  # Placeholder
        0.0,  # Placeholder
        0.0,  # Placeholder
        0.0   # Placeholder
    ]
    
    # Ensure we have exactly 7 inputs
    assert len(inputs) == 7, f"Expected 7 inputs, got {len(inputs)}"
    
    return inputs

def click_at(x, y, delay=0.1):
    """Click at the specified screen coordinates"""
    try:
        # Move mouse to position
        win32api.SetCursorPos((x, y))
        time.sleep(delay)
        
        # Left mouse button down
        win32api.mouse_event(win32con.MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0)
        time.sleep(delay)
        
        # Left mouse button up
        win32api.mouse_event(win32con.MOUSEEVENTF_LEFTUP, 0, 0, 0, 0)
        time.sleep(delay)
        
        logger.debug(f"Clicked at position ({x}, {y})")
        return True
    except Exception as e:
        logger.error(f"Failed to click at position ({x}, {y}): {e}")
        return False

def find_game_window():
    """Find the OpenGD game window and return its handle"""
    try:
        # Find window by title (partial match)
        window_handle = win32gui.FindWindow(None, "OpenGD")
        
        if window_handle == 0:
            # Try alternative window titles
            window_handle = win32gui.FindWindow(None, "Geometry Dash")
            
        if window_handle == 0:
            # Try finding any window with OpenGD in the title
            def callback(hwnd, windows):
                if "OpenGD" in win32gui.GetWindowText(hwnd):
                    windows.append(hwnd)
                return True
                
            windows = []
            win32gui.EnumWindows(callback, windows)
            
            if windows:
                window_handle = windows[0]
        
        if window_handle != 0:
            logger.info(f"Found game window with handle: {window_handle}")
            return window_handle
        else:
            logger.error("Could not find game window")
            return None
    except Exception as e:
        logger.error(f"Error finding game window: {e}")
        return None

def get_window_center(window_handle):
    """Get the center coordinates of a window"""
    try:
        # Get window rect
        left, top, right, bottom = win32gui.GetWindowRect(window_handle)
        width = right - left
        height = bottom - top
        
        # Calculate center
        center_x = left + width // 2
        center_y = top + height // 2
        
        logger.debug(f"Window dimensions: {width}x{height}, Center: ({center_x}, {center_y})")
        return center_x, center_y
    except Exception as e:
        logger.error(f"Error getting window center: {e}")
        return None

def activate_window(window_handle):
    """Bring the window to the foreground"""
    try:
        # Check if window is minimized
        if win32gui.IsIconic(window_handle):
            # Restore the window
            win32gui.ShowWindow(window_handle, win32con.SW_RESTORE)
            
        # Bring to foreground
        win32gui.SetForegroundWindow(window_handle)
        
        # Wait for window to activate
        time.sleep(0.5)
        
        logger.debug("Window activated")
        return True
    except Exception as e:
        logger.error(f"Error activating window: {e}")
        return False

def click_relative(window_handle, x_percent, y_percent):
    """Click at a position relative to window size (percentages)"""
    try:
        # Get window rect
        left, top, right, bottom = win32gui.GetWindowRect(window_handle)
        width = right - left
        height = bottom - top
        
        # Calculate absolute position
        x = left + int(width * x_percent)
        y = top + int(height * y_percent)
        
        # Click at position
        logger.debug(f"Clicking at relative position ({x_percent*100}%, {y_percent*100}%) -> ({x}, {y})")
        return click_at(x, y)
    except Exception as e:
        logger.error(f"Error clicking at relative position: {e}")
        return False

def click_play(window_handle):
    """Click the play button in the center of the window"""
    logger.info("Clicking play button")
    
    # Activate window
    if not activate_window(window_handle):
        return False
    
    # Wait for game to load
    time.sleep(2)
    
    # Click at the center of the window (play button is usually centered)
    return click_relative(window_handle, 0.5, 0.5)

def click_level(window_handle):
    """Click to select the first level"""
    logger.info("Clicking level selection")
    
    # Wait for menu to appear
    time.sleep(1)
    
    # Click at the position of the first level (adjust percentages as needed)
    # This is approximately where the first level would be
    return click_relative(window_handle, 0.5, 0.3)

def start_game():
    """Start the game and navigate to gameplay"""
    try:
        # Start the game process
        logger.info(f"Starting game from: {GAME_PATH}")
        game_process = subprocess.Popen([GAME_PATH])
        
        # Wait for game window to appear
        time.sleep(3)
        
        # Find game window
        window_handle = find_game_window()
        if window_handle is None:
            logger.error("Could not find game window")
            return None
            
        # Click through menus
        if not click_play(window_handle):
            logger.error("Failed to click play button")
            return None
            
        if not click_level(window_handle):
            logger.error("Failed to click level")
            return None
            
        # Wait for level to load
        time.sleep(2)
        
        logger.info("Game started successfully")
        return game_process
    except Exception as e:
        logger.error(f"Failed to start game: {e}")
        return None

def press_space():
    """Press the space key using win32api"""
    try:
        # Key down
        win32api.keybd_event(0x20, 0, 0, 0)  # 0x20 is the virtual key code for SPACE
        time.sleep(0.05)
        # Key up
        win32api.keybd_event(0x20, 0, win32con.KEYEVENTF_KEYUP, 0)
        logger.debug("Space key pressed using win32api")
    except Exception as e:
        logger.error(f"Failed to press space key: {e}")

def reset_game():
    """Reset the game by pressing R"""
    try:
        # Key down
        win32api.keybd_event(0x52, 0, 0, 0)  # 0x52 is the virtual key code for R
        time.sleep(0.05)
        # Key up
        win32api.keybd_event(0x52, 0, win32con.KEYEVENTF_KEYUP, 0)
        time.sleep(1)  # Wait for reset
        logger.debug("Game reset using R key")
    except Exception as e:
        logger.error(f"Failed to reset game: {e}")

def log_decision(inputs, output, should_jump, position):
    """Log neural network decision details for debugging"""
    if logger.isEnabledFor(logging.DEBUG):
        logger.debug(f"Position: x={position['x']:.2f}, y={position['y']:.2f}, y_vel={position['y_vel']:.2f}, on_ground={position['on_ground']}")
        logger.debug(f"Network inputs: {inputs}")
        logger.debug(f"Network output: {output[0]:.4f}, Decision: {'JUMP' if should_jump else 'NO JUMP'}")

def evaluate_genomes(genomes, config):
    """Evaluate all genomes in the population sequentially"""
    logger.info(f"Evaluating {len(genomes)} genomes sequentially")
    
    for i, (genome_id, genome) in enumerate(genomes):
        logger.info(f"Evaluating genome {i+1}/{len(genomes)} (ID: {genome_id})")
        genome.fitness = evaluate_genome(genome_id, genome, config)
        
        # Add a small delay between evaluations to ensure clean process termination
        time.sleep(2)

def evaluate_genome(genome_id, genome, config):
    """Evaluate a single genome"""
    logger.info(f"Evaluating genome {genome_id}")
    
    # Create neural network from genome
    net = neat.nn.FeedForwardNetwork.create(genome, config)
    
    # Start game
    logger.info(f"Starting game for genome {genome_id}")
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
    
    # Debug: Check if position file exists before starting
    logger.info(f"Looking for position file at: {POSITION_FILE}")
    if os.path.exists(POSITION_FILE):
        logger.info(f"Position file found before game start")
    else:
        logger.info(f"Position file not found before game start")
    
    # Wait a moment for the game to initialize
    logger.info("Waiting for game to initialize...")
    time.sleep(3)
    
    try:
        # Debug: Check if position file exists after waiting
        if os.path.exists(POSITION_FILE):
            logger.info(f"Position file found after waiting")
            with open(POSITION_FILE, 'r') as f:
                logger.info(f"Initial file content: {f.read().strip()}")
        else:
            logger.info(f"Position file still not found after waiting")
        
        # Main game loop
        frame_count = 0
        position_read_count = 0
        logger.info("Starting main evaluation loop")
        
        while True:
            frame_count += 1
            
            # Check for timeout
            current_time = time.time()
            elapsed = current_time - start_time
            if elapsed > TIMEOUT_SECONDS:
                logger.info(f"Timeout reached for genome {genome_id} after {elapsed:.2f} seconds")
                break
                
            # Get player position
            position = get_player_position()
            if position is None:
                if frame_count % 20 == 0:  # Only log every 20 frames to avoid spam
                    logger.debug(f"No position data available (frame {frame_count})")
                time.sleep(0.05)
                continue
            
            position_read_count += 1
            if position_read_count == 1:
                logger.info(f"First position data received: x={position['x']:.2f}, y={position['y']:.2f}")
                
            # Update history
            update_position_history(position)
            
            # Check if player is dead
            if position['is_dead']:
                logger.info(f"Player died at x={position['x']:.2f}, y={position['y']:.2f}")
                break
                
            # Update max_x for fitness
            if position['x'] > max_x:
                if int(position['x']) % 100 == 0:  # Log every 100 units
                    logger.info(f"Reached x={position['x']:.2f}")
                max_x = position['x']
                
            # Prepare inputs for neural network
            inputs = prepare_inputs()
            if inputs is None or len(position_history) < 2:  # Need at least 2 positions for meaningful input
                logger.debug("Insufficient position history for inputs")
                continue
                
            # Get neural network output
            output = net.activate(inputs)
            
            # Decide whether to jump
            should_jump = output[0] > 0.5
            
            # Log decision details (every 10 frames to avoid spam)
            if frame_count % 10 == 0:
                logger.debug(f"Network output: {output[0]:.4f}, Should jump: {should_jump}")
            
            # Execute jump if needed (with cooldown to prevent spam)
            if should_jump and current_time - last_jump_time > 0.2:
                logger.info(f"Jumping at x={position['x']:.2f}, y={position['y']:.2f}, output={output[0]:.4f}")
                press_space()  # Use our win32api function
                jumps += 1
                last_jump_time = current_time
                
            # Record this frame's data
            genome_log.append({
                'time': elapsed,
                'x': position['x'],
                'y': position['y'],
                'y_vel': position['y_vel'],
                'on_ground': position['on_ground'],
                'output': float(output[0]),  # Convert numpy float to Python float for JSON serialization
                'jumped': should_jump and current_time - last_jump_time <= 0.2
            })
                
            time.sleep(0.05)  # Small delay to prevent CPU overuse
            
    except Exception as e:
        logger.error(f"Error during evaluation of genome {genome_id}: {e}", exc_info=True)
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
        winner = population.run(evaluate_genomes, MAX_GENERATIONS)
        
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
