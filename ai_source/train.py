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
import win32ui
from PIL import Image

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
<<<<<<< HEAD
    """Convert position history to neural network inputs - simplified to just use x position"""
    if not position_history or len(position_history) < 2:
=======
    """Prepare inputs for the neural network"""
    if len(position_history) < 2:
>>>>>>> eca28ee80f1d6f0bd903266d871329e9156f6415
        return None
        
    # Get current and previous positions
    current = position_history[0]
    prev = position_history[1]
    
<<<<<<< HEAD
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
=======
    # Calculate velocity
    dx = current['x'] - prev['x']
    dy = current['y'] - prev['y']
    
    # Prepare inputs (make sure this matches the number in config-feedforward.txt)
    inputs = [
        current['x'] / 1000.0,  # Normalize x position
        current['y'] / 1000.0,  # Normalize y position
        current['y_vel'] / 20.0,  # Normalize y velocity
        1.0 if current['on_ground'] else 0.0,  # Ground state
        dx / 10.0,  # x velocity
        dy / 10.0,  # y velocity
        1.0 if current['is_dead'] else 0.0  # Death state
    ]
    
    return inputs

def terminate_all_game_processes():
    """Find and terminate all running OpenGD processes"""
    try:
        logger.info("Searching for OpenGD processes to terminate...")
        found_processes = 0
        
        for proc in psutil.process_iter(['pid', 'name']):
            # Look for OpenGD processes
            if 'OpenGD' in proc.info['name']:
                found_processes += 1
                pid = proc.info['pid']
                logger.info(f"Found OpenGD process (PID: {pid}, Name: {proc.info['name']})")
                
                try:
                    process = psutil.Process(pid)
                    logger.info(f"Attempting to terminate process {pid}...")
                    process.terminate()
                    
                    # Wait for termination with timeout
                    logger.info(f"Waiting for process {pid} to terminate...")
                    process.wait(timeout=3)
                    logger.info(f"Process {pid} terminated successfully")
                    
                except psutil.NoSuchProcess as e:
                    logger.warning(f"Process {pid} no longer exists: {e}")
                except psutil.AccessDenied as e:
                    logger.warning(f"Access denied when terminating process {pid}: {e}")
                    logger.info(f"Attempting to kill process {pid} forcefully...")
                    try:
                        process.kill()
                        logger.info(f"Process {pid} killed forcefully")
                    except Exception as kill_error:
                        logger.error(f"Failed to kill process {pid}: {kill_error}")
                except psutil.TimeoutExpired as e:
                    logger.warning(f"Timeout waiting for process {pid} to terminate: {e}")
                    logger.info(f"Attempting to kill process {pid} forcefully...")
                    try:
                        process.kill()
                        logger.info(f"Process {pid} killed forcefully")
                    except Exception as kill_error:
                        logger.error(f"Failed to kill process {pid}: {kill_error}")
                except Exception as e:
                    logger.error(f"Unexpected error terminating process {pid}: {e}")
                    logger.info(f"Attempting to kill process {pid} forcefully...")
                    try:
                        process.kill()
                        logger.info(f"Process {pid} killed forcefully")
                    except Exception as kill_error:
                        logger.error(f"Failed to kill process {pid}: {kill_error}")
        
        if found_processes == 0:
            logger.info("No OpenGD processes found to terminate")
        else:
            logger.info(f"Attempted to terminate {found_processes} OpenGD processes")
        
        # Double-check if any OpenGD processes are still running
        remaining = 0
        for proc in psutil.process_iter(['pid', 'name']):
            if 'OpenGD' in proc.info['name']:
                remaining += 1
                logger.warning(f"OpenGD process still running after termination attempts: PID {proc.info['pid']}")
        
        if remaining > 0:
            logger.warning(f"{remaining} OpenGD processes still running after termination attempts")
            
            # Last resort: use taskkill on Windows
            if platform.system() == 'Windows':
                logger.info("Attempting to use taskkill to forcefully terminate remaining processes")
                try:
                    subprocess.run(['taskkill', '/F', '/IM', 'OpenGD.exe'], 
                                  stdout=subprocess.PIPE, 
                                  stderr=subprocess.PIPE)
                    logger.info("taskkill command executed")
                except Exception as e:
                    logger.error(f"Error using taskkill: {e}")
        else:
            logger.info("All OpenGD processes successfully terminated")
        
        # Small delay to ensure processes are fully terminated
        logger.info("Waiting 2 seconds for processes to fully terminate...")
        time.sleep(2)
        return True
    except Exception as e:
        logger.error(f"Error in terminate_all_game_processes: {e}")
        return False

def start_game():
    """Start the OpenGD game after ensuring no other instances are running"""
    # First terminate any existing game processes
    logger.info("Terminating any existing OpenGD processes before starting new game...")
    terminate_all_game_processes()
    
    logger.info(f"Starting OpenGD from {GAME_PATH}...")
    try:
        # Get the directory containing the game executable
        game_dir = os.path.dirname(GAME_PATH)
        executable = os.path.basename(GAME_PATH)
        
        # Store current directory to return to later
        original_dir = os.getcwd()
        
        # Change to game directory
        os.chdir(game_dir)
        logger.info(f"Changed to game directory: {game_dir}")
        
        # Start the game using just the executable name
        logger.info(f"Launching executable: {executable}")
        process = subprocess.Popen(executable, shell=True)
        logger.info(f"Game process started with PID: {process.pid if hasattr(process, 'pid') else 'unknown'}")
        
        logger.info("Waiting 5 seconds for game to initialize...")
        time.sleep(5)  # Wait for game to start
        
        # Change back to original directory
        os.chdir(original_dir)
        logger.info(f"Changed back to original directory: {original_dir}")
>>>>>>> eca28ee80f1d6f0bd903266d871329e9156f6415
        
        logger.info("Game started successfully")
        return game_process
    except Exception as e:
        logger.error(f"Failed to start game: {e}")
        # Make sure we return to original directory even if there's an error
        try:
            if 'original_dir' in locals():
                os.chdir(original_dir)
                logger.info(f"Changed back to original directory after error: {original_dir}")
        except Exception as dir_error:
            logger.error(f"Failed to change back to original directory: {dir_error}")
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

<<<<<<< HEAD
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
=======
def capture_window_image(hwnd):
    """Capture a screenshot of the specified window"""
    try:
        # Get window dimensions
        left, top, right, bottom = win32gui.GetWindowRect(hwnd)
        width = right - left
        height = bottom - top
        
        # Create device context
        hwnd_dc = win32gui.GetWindowDC(hwnd)
        mfc_dc = win32ui.CreateDCFromHandle(hwnd_dc)
        save_dc = mfc_dc.CreateCompatibleDC()
        
        # Create bitmap
        save_bitmap = win32ui.CreateBitmap()
        save_bitmap.CreateCompatibleBitmap(mfc_dc, width, height)
        save_dc.SelectObject(save_bitmap)
        
        # Copy screen to bitmap
        save_dc.BitBlt((0, 0), (width, height), mfc_dc, (0, 0), win32con.SRCCOPY)
        
        # Convert to numpy array
        bmpinfo = save_bitmap.GetInfo()
        bmpstr = save_bitmap.GetBitmapBits(True)
        img = np.frombuffer(bmpstr, dtype=np.uint8)
        img.shape = (bmpinfo['bmHeight'], bmpinfo['bmWidth'], 4)
        img = img[:, :, :3]  # Remove alpha channel
        
>>>>>>> eca28ee80f1d6f0bd903266d871329e9156f6415
        # Clean up
        win32gui.DeleteObject(save_bitmap.GetHandle())
        save_dc.DeleteDC()
        mfc_dc.DeleteDC()
        win32gui.ReleaseDC(hwnd, hwnd_dc)
        
        return img
    except Exception as e:
        logger.error(f"Error capturing window image: {e}")
        return None

def is_death_detected(hwnd, num_frames=8, similarity_threshold=0.999):
    """Detect death by checking for freeze frames"""
    frames = []
    
    # Capture initial frames
    for _ in range(num_frames):
        img = capture_window_image(hwnd)
        if img is None:
            return False
        frames.append(img)
        time.sleep(0.05)  # Small delay between captures
    
    # Calculate similarity between consecutive frames
    similarities = []
    for i in range(1, len(frames)):
        # Convert to grayscale for simpler comparison
        prev_gray = cv2.cvtColor(frames[i-1], cv2.COLOR_BGR2GRAY)
        curr_gray = cv2.cvtColor(frames[i], cv2.COLOR_BGR2GRAY)
        
        # Calculate similarity (normalized cross-correlation)
        result = cv2.matchTemplate(prev_gray, curr_gray, cv2.TM_CCORR_NORMED)
        similarity = result[0][0]
        similarities.append(similarity)
    
    # Check if all similarities are above threshold
    all_similar = all(sim >= similarity_threshold for sim in similarities)
    
    if all_similar:
        logger.info(f"Death detected - screen frozen for {num_frames} frames")
        return True
    
    return False

def find_game_window():
    """Find the OpenGD window with more robust detection"""
    try:
        # Try different possible window titles
        window_titles = ["OpenGD", "OpenGD - Geometry Dash", "Geometry Dash"]
        
        for title in window_titles:
            hwnd = win32gui.FindWindow(None, title)
            if hwnd != 0:
                logger.info(f"Found game window with title: {title}")
                return hwnd
                
        # If not found by title, try finding by class
        def callback(hwnd, hwnds):
            if win32gui.IsWindowVisible(hwnd) and "OpenGD" in win32gui.GetWindowText(hwnd):
                hwnds.append(hwnd)
            return True
            
        hwnds = []
        win32gui.EnumWindows(callback, hwnds)
        
        if hwnds:
            logger.info(f"Found game window by partial title match")
            return hwnds[0]
            
        logger.error("Game window not found")
        return 0
    except Exception as e:
        logger.error(f"Error finding game window: {e}")
        return 0

def evaluate_genome(genomes, config):
    """Evaluate the fitness of each genome"""
    for genome_id, genome in genomes:
        logger.info(f"===== Starting evaluation of genome {genome_id} =====")
        
        # Initialize fitness metrics outside the try block
        max_x = 0
        jumps = 0
        
        # Create neural network
        net = neat.nn.FeedForwardNetwork.create(genome, config)
        logger.info(f"Created neural network for genome {genome_id}")
        
        # Start game process (this will terminate any existing games)
        logger.info(f"Starting game for genome {genome_id}...")
        game_process = start_game()
        if game_process is None:
            logger.error("Failed to start game")
            genome.fitness = 0
            continue
        
        logger.info(f"Game process info: {game_process}")
            
        try:
            # Navigate menus
            logger.info("Attempting to click play button...")
            if not click_play():
                logger.error("Failed to enter play mode")
                genome.fitness = 0
                continue
                
            logger.info("Attempting to click level...")
            if not click_level():
                logger.error("Failed to select level")
                genome.fitness = 0
                continue
            
            # Get window handle for death detection
            logger.info("Finding game window for death detection...")
            hwnd = find_game_window()
            if hwnd == 0:
                logger.error("Could not find game window for death detection")
                genome.fitness = 0
                continue
            logger.info(f"Found game window with handle: {hwnd}")
            
            # Clear position history
            position_history.clear()
            logger.info("Cleared position history")
            
            # Initialize timing variables
            start_time = time.time()
            last_jump_time = 0
            
            # Initialize frame comparison variables
            prev_frame = None
            similar_frames_count = 0
            similarity_threshold = 0.999
            
            logger.info("Starting main game loop...")
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
                
                # Check if player is dead from position data
                if position['is_dead']:
                    logger.info(f"Player death detected from position data at x={position['x']:.2f}")
                    break
                
                # Capture current frame for death detection
                current_frame = capture_window_image(hwnd)
                if current_frame is not None and prev_frame is not None:
                    # Convert to grayscale for simpler comparison
                    prev_gray = cv2.cvtColor(prev_frame, cv2.COLOR_BGR2GRAY)
                    curr_gray = cv2.cvtColor(current_frame, cv2.COLOR_BGR2GRAY)
                    
                    # Calculate similarity (normalized cross-correlation)
                    result = cv2.matchTemplate(prev_gray, curr_gray, cv2.TM_CCORR_NORMED)
                    similarity = result[0][0]
                    
                    # Check for frozen frames (indicating death)
                    if similarity >= similarity_threshold:
                        similar_frames_count += 1
                        if similar_frames_count >= 8:  # 8 consecutive similar frames
                            logger.info(f"Death detected - screen frozen for {similar_frames_count} frames (similarity: {similarity:.6f})")
                            break
                    else:
                        # Reset counter if frames are different
                        similar_frames_count = 0
                
                # Update previous frame
                prev_frame = current_frame
                
                # Update max_x for fitness
                if position['x'] > max_x:
                    max_x = position['x']
                
                # Prepare inputs for neural network
                inputs = prepare_inputs()
                if inputs is None:
                    continue
                    
                # Get neural network output
                output = net.activate(inputs)
                
                # Decide whether to jump
                should_jump = output[0] > 0.5
                
                # Execute jump if needed (with cooldown to prevent spam)
                if should_jump and current_time - last_jump_time > 0.2:
                    win32api.keybd_event(win32con.VK_SPACE, 0, 0, 0)  # Space down
                    time.sleep(0.05)
                    win32api.keybd_event(win32con.VK_SPACE, 0, win32con.KEYEVENTF_KEYUP, 0)  # Space up
                    jumps += 1
                    last_jump_time = current_time
                    logger.debug(f"Jump executed at x={position['x']:.2f}, y={position['y']:.2f}")
                
                time.sleep(0.05)  # Small delay to prevent CPU overuse
                
        except Exception as e:
            logger.error(f"Error during evaluation of genome {genome_id}: {e}")
            import traceback
            logger.error(f"Traceback: {traceback.format_exc()}")
        finally:
            # Clean up - terminate the game process and any other OpenGD processes
            logger.info(f"Cleaning up after genome {genome_id} evaluation...")
            try:
                # First try to terminate the process we started
                if game_process:
                    logger.info(f"Terminating game process with PID: {game_process.pid if hasattr(game_process, 'pid') else 'unknown'}")
                    game_process.terminate()
                    logger.info("Game process terminate() called")
                    time.sleep(1)
                    
                    # Check if process is still running
                    if hasattr(game_process, 'poll'):
                        exit_code = game_process.poll()
                        if exit_code is None:
                            logger.warning("Game process did not terminate, attempting to kill")
                            game_process.kill()
                            logger.info("Game process kill() called")
                        else:
                            logger.info(f"Game process terminated with exit code: {exit_code}")
                
                # Then make sure all OpenGD processes are terminated
                logger.info("Ensuring all OpenGD processes are terminated...")
                terminate_all_game_processes()
            except Exception as e:
                logger.error(f"Error terminating game processes: {e}")
                import traceback
                logger.error(f"Traceback: {traceback.format_exc()}")
        
        # Calculate fitness
        fitness = max_x
        
        # Set the fitness for this genome
        genome.fitness = fitness
        
        # Log evaluation results
        logger.info(f"Genome {genome_id} evaluation complete:")
        logger.info(f"  - Max distance: {max_x:.2f}")
        logger.info(f"  - Jumps: {jumps}")
        logger.info(f"  - Final fitness: {fitness:.2f}")
        
        # Save detailed genome run data
        try:
            genome_file = os.path.join(genome_data_dir, f"genome_{genome_id}_{int(fitness)}.json")
            with open(genome_file, 'w') as f:
                json.dump({
                    'genome_id': genome_id,
                    'fitness': fitness,
                    'max_x': max_x,
                    'jumps': jumps
                }, f, indent=2)
            logger.debug(f"Saved detailed genome data to {genome_file}")
        except Exception as e:
            logger.error(f"Failed to save genome data: {e}")
            
        logger.info(f"===== Completed evaluation of genome {genome_id} =====")

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

def click_play():
    """Click the play button in the middle of the OpenGD window"""
    try:
        # Find the OpenGD window
        hwnd = win32gui.FindWindow(None, "OpenGD")
        if hwnd == 0:
            logger.error("OpenGD window not found")
            return False
            
        # Get window dimensions
        rect = win32gui.GetWindowRect(hwnd)
        width = rect[2] - rect[0]
        height = rect[3] - rect[1]
        
        # Calculate center position
        center_x = rect[0] + width // 2
        center_y = rect[1] + height // 2
        
        # Click at center (play button)
        win32api.SetCursorPos((center_x, center_y))
        win32api.mouse_event(win32con.MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0)
        time.sleep(0.1)
        win32api.mouse_event(win32con.MOUSEEVENTF_LEFTUP, 0, 0, 0, 0)
        
        logger.info(f"Clicked play button at ({center_x}, {center_y})")
        time.sleep(1)  # Wait for menu transition
        
    except Exception as e:
        logger.error(f"Failed to click play button: {e}")
        return False
    return True

def click_level(level_number=1):
    """Click the level button in the middle-top area of the OpenGD window"""
    try:
        # Find the OpenGD window
        hwnd = win32gui.FindWindow(None, "OpenGD")
        if hwnd == 0:
            logger.error("OpenGD window not found")
            return False
            
        # Get window dimensions
        rect = win32gui.GetWindowRect(hwnd)
        width = rect[2] - rect[0]
        height = rect[3] - rect[1]
        
        # Calculate position (center horizontally, 40% from top vertically)
        level_x = rect[0] + width // 2
        level_y = rect[1] + int(height * 0.4)  # Slightly above center
        
        # Click at level position
        win32api.SetCursorPos((level_x, level_y))
        win32api.mouse_event(win32con.MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0)
        time.sleep(0.1)
        win32api.mouse_event(win32con.MOUSEEVENTF_LEFTUP, 0, 0, 0, 0)
        
        logger.info(f"Clicked level {level_number} at ({level_x}, {level_y})")
        time.sleep(2)  # Wait for level to load
        
    except Exception as e:
        logger.error(f"Failed to click level: {e}")
        return False
    return True

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
