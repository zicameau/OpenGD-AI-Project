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

# Platform-specific imports
IS_WINDOWS = platform.system() == 'Windows'
if IS_WINDOWS:
    import win32gui
    import win32con
else:
    import Xlib.display
    from Xlib import X

# Load config file
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CONFIG_PATH = os.path.join(SCRIPT_DIR, "config.json")

with open(CONFIG_PATH, "r") as f:
    config = json.load(f)

# Get platform-specific game path
platform_name = platform.system().lower()
if isinstance(config["game_path"], dict):
    GAME_PATH = os.path.abspath(os.path.join(SCRIPT_DIR, "..", config["game_path"].get(platform_name, "")))
else:
    GAME_PATH = os.path.abspath(os.path.join(SCRIPT_DIR, "..", config["game_path"]))

frame_history = []
max_history = 5  # Number of frames to store

def update_frame_history(frame):
    if frame is None:
        print("⚠️ Warning: Attempted to update frame history with None frame")
        return
        
    if len(frame_history) >= max_history:
        frame_history.pop(0)  # Remove oldest frame
    
    processed = preprocess_frame(frame)
    frame_history.append(processed)

def get_network_input():
    """Flatten and concatenate the last few frames for NEAT input."""
    while len(frame_history) < max_history:
        frame_history.append(np.zeros(40 * 10))  # Fill with empty frames initially
    return np.concatenate(frame_history)

def start_game():
    """Start OpenGD if not already running."""
    # Check if OpenGD is already running
    for process in psutil.process_iter(attrs=['pid', 'name']):
        if "OpenGD.exe" in process.info['name']:
            print("✅ OpenGD is already running.")
            
            # Bring the window to foreground if it's already running
            hwnd = get_window_handle()
            if hwnd:
                try:
                    win32gui.SetForegroundWindow(hwnd)
                    print("✅ Brought OpenGD window to foreground")
                except Exception as e:
                    print(f"⚠️ Could not bring window to foreground: {e}")
            
            return True  # Return True if already running

    print(f"🚀 Attempting to start OpenGD from: {GAME_PATH}")

    if not os.path.exists(GAME_PATH):
        print("❌ ERROR: OpenGD.exe not found.")
        return False

    try:
        # ✅ Use cmd /c to launch OpenGD in the same way the terminal does
        game_dir = os.path.dirname(GAME_PATH)
        original_dir = os.getcwd()
        os.chdir(game_dir)
        process = subprocess.Popen(
            f'cmd /c "{GAME_PATH}"',
            shell=True
        )
        # Change back to original directory
        os.chdir(original_dir)

        print("✅ OpenGD launched successfully! Waiting to verify...")
        time.sleep(5)  # Give OpenGD time to initialize

        # Check if OpenGD is running and bring to foreground
        for process in psutil.process_iter(attrs=['pid', 'name']):
            if "OpenGD.exe" in process.info['name']:
                print("✅ OpenGD is running!")
                
                # Bring the window to foreground
                hwnd = get_window_handle()
                if hwnd:
                    try:
                        win32gui.SetForegroundWindow(hwnd)
                        print("✅ Brought OpenGD window to foreground")
                    except Exception as e:
                        print(f"⚠️ Could not bring window to foreground: {e}")
                
                return True  # Game started successfully

        print("❌ ERROR: OpenGD crashed immediately.")
        return False

    except Exception as e:
        print(f"❌ ERROR launching OpenGD: {e}")
        return False

def check_game_runs():
    """Ensures that OpenGD can start and run before proceeding."""
    print("🔎 Checking if OpenGD launches successfully...")

    if not start_game():
        print("❌ ERROR: OpenGD did not start properly.")
        print("🛑 Exiting program...")
        sys.exit(1)  # Exit if game cannot run

    print("✅ OpenGD is verified and running! Proceeding with AI training...")

def get_window_handle():
    """Get the OpenGD window handle using platform-specific code."""
    if IS_WINDOWS:
        def callback(hwnd, windows):
            if win32gui.IsWindowVisible(hwnd) and "OpenGD" == win32gui.GetWindowText(hwnd):
                windows.append(hwnd)
            return True
        
        windows = []
        win32gui.EnumWindows(callback, windows)
        return windows[0] if windows else None
    else:
        display = Xlib.display.Display()
        root = display.screen().root
        window_ids = root.get_full_property(
            display.intern_atom('_NET_CLIENT_LIST'),
            X.AnyPropertyType
        ).value
        
        for window_id in window_ids:
            window = display.create_resource_object('window', window_id)
            name = window.get_wm_name()
            if name and "OpenGD" in name:
                return window
        return None

def click_play_button():
    """Locate and click the play button in OpenGD."""
    try:
        hwnd = get_window_handle()
        
        # Get window dimensions
        rect = win32gui.GetWindowRect(hwnd)
        x = rect[0]
        y = rect[1]
        width = rect[2] - x
        height = rect[3] - y
        
        # Calculate play button position (approximately center of window)
        play_x = x + (width // 2)
        play_y = y + (height // 2)
        
        # Move mouse and click
        pyautogui.click(play_x, play_y)
        print("🎮 Clicked play button")
        time.sleep(1)  # Give the game time to respond
        
    except Exception as e:
        print(f"❌ ERROR clicking play button: {str(e)}")

def click_level_button():
    """Locate and click the level button in OpenGD."""
    try:
        hwnd = get_window_handle()
        
        # Get window dimensions
        rect = win32gui.GetWindowRect(hwnd)
        x = rect[0]
        y = rect[1]
        width = rect[2] - x
        height = rect[3] - y
        
        # Calculate level button position (slightly above center)
        play_x = x + (width // 2)
        play_y = y + (height // 2) - 50  # 50 pixels above center
        
        # Move mouse and click
        pyautogui.click(play_x, play_y)
        print("🎮 Clicked level button")
        time.sleep(1)  # Give the game time to respond
        
    except Exception as e:
        print(f"❌ ERROR clicking level button: {str(e)}")

def capture_screen():
    """Capture the OpenGD window and return a processed image."""
    hwnd = get_window_handle()
    if not hwnd:
        return None

    rect = win32gui.GetWindowRect(hwnd)
    x, y, width, height = rect[0], rect[1], rect[2] - rect[0], rect[3] - rect[1]

    with mss.mss() as sct:
        screenshot = sct.grab({"top": y, "left": x, "width": width, "height": height})
        img = np.array(screenshot)
        gray_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    return gray_img

def preprocess_frame(frame, prev_frame=None):
    """
    Process a frame for neural network input.
    Shows the right half of the original frame to the user for inspection.
    """
    try:
        # Check if frame is valid
        if frame is None or frame.size == 0:
            print("⚠️ Warning: Invalid frame received in preprocess_frame")
            # Return a default array of zeros
            return np.zeros(400)
        
        # Get the dimensions of the frame
        height, width = frame.shape[:2]
        
        # Extract the right half of the screen, but check dimensions first
        start_x = min(width // 2, width - 1)  # Ensure we don't go out of bounds
        if start_x < width:
            right_half = frame[:, start_x:].copy()
            
            # Ensure right_half is not empty
            if right_half.size == 0:
                print("⚠️ Warning: Right half extraction resulted in empty array")
                right_half = frame.copy()  # Use full frame as fallback
        else:
            print("⚠️ Warning: Cannot extract right half, using full frame")
            right_half = frame.copy()
        
        # Display the frame (safely)
        try:
            
            # Process for neural network
            if len(frame.shape) > 2:
                gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            else:
                gray = frame.copy()
            
            # Extract right half for processing
            right_half_gray = gray[:, start_x:]
            
            # Resize to 20x20
            resized = cv2.resize(right_half_gray, (20, 20))
            edges = cv2.Canny(resized, 100, 200)
            normalized = edges.astype(np.float32) / 255.0
            flattened = normalized.flatten()
            
            return flattened
            
        except Exception as e:
            print(f"⚠️ Display error in preprocess_frame: {e}")
            # Continue with processing even if display fails
            
            # Process for neural network (fallback)
            if len(frame.shape) > 2:
                gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
            else:
                gray = frame.copy()
            
            resized = cv2.resize(gray, (20, 20))
            edges = cv2.Canny(resized, 100, 200)
            normalized = edges.astype(np.float32) / 255.0
            flattened = normalized.flatten()
            
            return flattened
            
    except Exception as e:
        print(f"❌ Critical error in preprocess_frame: {e}")
        # Return a default array of zeros
        return np.zeros(400)

def is_key_pressed(key_code=win32con.VK_SPACE):
    """Check if a key is currently pressed."""
    try:
        # Get the key state (high bit = 1 if key is pressed)
        key_state = win32api.GetAsyncKeyState(key_code)
        # The high bit (sign bit) indicates if the key is currently pressed
        return (key_state & 0x8000) != 0
    except Exception as e:
        print(f"❌ Error checking key state: {e}")
        return False

def ensure_key_up(key_code=win32con.VK_SPACE, max_attempts=5):
    """Send key up events until the key is detected as up or max attempts reached."""
    hwnd = get_window_handle()
    if not hwnd:
        return False
        
    attempts = 0
    while is_key_pressed(key_code) and attempts < max_attempts:
        try:
            win32gui.PostMessage(hwnd, win32con.WM_KEYUP, key_code, 0)
            print(f"📤 Sent additional key up (attempt {attempts+1})")
            time.sleep(0.01)
            attempts += 1
        except Exception as e:
            print(f"❌ Error sending key up: {e}")
            break
            
    return not is_key_pressed(key_code)

def jump():
    """Simulate spacebar press using a more direct approach."""
    try:
        # Get the window handle first
        hwnd = get_window_handle()
        if not hwnd:
            print("❌ Could not find OpenGD window")
            return False
        
        # Make sure the window is active
        try:
            # Bring window to foreground
            win32gui.SetForegroundWindow(hwnd)
            time.sleep(0.01)  # Small delay to ensure window is active
            
            # Use a more direct approach with keybd_event
            VK_SPACE = 0x20
            KEYEVENTF_KEYUP = 0x0002
            
            # Send key down
            print("🔍 Sending key down via keybd_event")
            ctypes.windll.user32.keybd_event(VK_SPACE, 0, 0, 0)
            time.sleep(0.05)  # Delay between down and up
            
            # Send key up
            print("🔍 Sending key up via keybd_event")
            ctypes.windll.user32.keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0)
            
            print("✅ Jump executed")
            return True
            
        except Exception as e:
            print(f"❌ Jump method failed: {e}")
            return False
            
    except Exception as e:
        print(f"❌ ERROR in jump(): {e}")
        return False

def close_game():
    """Close OpenGD using platform-specific commands."""
    if IS_WINDOWS:
        try:
            subprocess.run(['taskkill', '/F', '/IM', 'OpenGD.exe'], check=True)
        except subprocess.CalledProcessError as e:
            print(f"❌ ERROR closing game: {e}")
    else:
        try:
            subprocess.run(['pkill', '-f', 'OpenGD'], check=True)
        except subprocess.CalledProcessError as e:
            print(f"❌ ERROR closing game: {e}")
    print("🎮 Closed OpenGD")
    time.sleep(1)

def train_genome(genome_config):
    genome, config = genome_config
    net = neat.nn.FeedForwardNetwork.create(genome, config)
    
    # Debug network structure in detail
    print(f"\n🧠 Network Structure for Genome {genome.key}:")
    print(f"Input nodes: {config.genome_config.input_keys}")
    print(f"Output nodes: {config.genome_config.output_keys}")
    print(f"Total nodes: {len(genome.nodes)}")
    print(f"Total connections: {len(genome.connections)}")
    
    # Print all node biases
    print("\nNode biases:")
    for node_id, node in genome.nodes.items():
        print(f"Node {node_id}: bias={node.bias:.4f}")
    
    # Reduce the jump threshold to make jumping more selective
    jump_threshold = 0.7  # Increase from 0.5 to 0.7
    
    # Increase jump cooldown to prevent constant jumping
    jump_cooldown = 0.8  # Increase from 0.5 to 0.8 seconds
    
    # Add randomness to jumping to encourage exploration
    jump_randomness = 0.2  # 20% random chance to override network decision
    
    fitness = 0
    start_time = time.time()
    successful_jumps = 0
    prev_frame = None
    last_jump_time = 0
    last_jump_decision = 0  # 0 = no jump, 1 = jump
    
    # Variables for static screen detection
    static_frame_counter = 0
    static_threshold = 0.999  # 98% similarity indicates a static screen
    
    # Add frame counter to delay death detection at the start
    frame_counter = 0
    startup_delay_frames = 30  # Wait for 100 frames before enabling death detection
    
    # Flag to track whether we've clicked the level button yet
    level_button_clicked = False
    
    try:
        # Click play button
        click_play_button()
        
        while True:
            # Capture frame
            frame = capture_screen()
            if frame is None:
                break
                
            # Click level button if we haven't done so yet
            if not level_button_clicked:
                click_level_button()
                level_button_clicked = True
                print("🎮 Level button clicked, now AI can start playing")
                continue  # Skip the rest of this iteration to get a fresh frame
            
            # Increment frame counter
            frame_counter += 1
            current_time = time.time() - start_time  # Time since start of run
            
            # Detect death by checking for static screens, but only after startup delay
            if prev_frame is not None and frame_counter > startup_delay_frames:
                # Calculate similarity between current and previous frame
                # Using normalized cross-correlation for better similarity measure
                similarity = cv2.matchTemplate(frame, prev_frame, cv2.TM_CCORR_NORMED)[0][0]
                print(f"Frame similarity: {similarity:.4f}")
                
                # If frames are very similar (static), increment counter
                if similarity > static_threshold:
                    static_frame_counter += 1
                    print(f"Static frame detected ({static_frame_counter}/10)")
                else:
                    static_frame_counter = 0
                
                # If we detect 10 consecutive static frames, assume death/restart screen
                if static_frame_counter >= 8:
                    print("💀 AI Died! Static screen detected for too long.")
                    close_game()
                    break
            elif frame_counter <= startup_delay_frames:
                print(f"Startup delay: {frame_counter}/{startup_delay_frames} frames")

            features = preprocess_frame(frame, prev_frame)

            prev_frame = frame.copy()
            
            
            # Add last jump decision and time information to inputs
            time_since_last_jump = current_time - last_jump_time
            inputs = np.append(features, [last_jump_decision, time_since_last_jump])
            
            output = net.activate(inputs)
            print(f"Network output: {output[0]:.4f}")

            # Check if enough time has passed since the last jump
            can_jump = time_since_last_jump > jump_cooldown

            # Only allow jumping if the cooldown period has passed AND output exceeds threshold
            if output[0] > jump_threshold and can_jump:
                # Check if key is already pressed before jumping
                if is_key_pressed(win32con.VK_SPACE):
                    print("⚠️ Space key already pressed, releasing first")
                    ensure_key_up(win32con.VK_SPACE)
                
                # Now perform the jump
                jump_probability = np.random.uniform(0, 1)  # Generate a random number
                if jump_probability < output[0]:  # AI jumps with probability equal to output
                    jump()
                    last_jump_time = time.time()
                    last_jump_decision = 1

                    successful_jumps += 1
                    last_jump_decision = 1
                    last_jump_time = current_time
                    print(f"🚀 AI Jumped! Output: {output[0]:.4f}")
                    fitness += 5  # Reward for jumping
            elif not can_jump and output[0] > jump_threshold:
                print(f"🕒 Jump cooldown active: {time_since_last_jump:.2f}s/{jump_cooldown:.2f}s (Output: {output[0]:.4f})")
                # Still record the decision even though we didn't actually jump
                last_jump_decision = 1
            else:
                last_jump_decision = 0
            
            # Calculate fitness primarily based on survival time
            # Each frame survived adds to fitness (100 frames = 1 second ≈ 100 fitness points)
            fitness = frame_counter * 1.0
            
            # Add a small bonus for distance traveled (approximated by time survived)
            # This creates a continuous gradient for the fitness function
            if frame_counter > startup_delay_frames:
                effective_time = frame_counter - startup_delay_frames
                # Exponential reward for surviving longer
                fitness += (effective_time ** 1.5) * 0.01
            
            time.sleep(0.005)
            
            if time.time() - start_time > 50000:
                print("⏱️ Time limit reached")
                close_game()
                break
        
        return max(fitness, 1)

    except Exception as e:
        print(f"❌ ERROR in train_genome(): {e}")
        close_game()  # Make sure game is closed even on error
        return 1

    finally:
        cv2.destroyAllWindows()

def eval_genome(genomes, config):
    """Evaluate all genomes."""
    total_genomes = len(genomes)
    
    for i, (genome_id, genome) in enumerate(genomes):
        print(f"\n🧬 Evaluating Genome {i+1}/{total_genomes} (ID: {genome_id}) in Generation {eval_genome.current_generation}")
        
        if not start_game():
            print("❌ ERROR: OpenGD did not start properly. Skipping genome evaluation.")
            genome.fitness = 0
            continue

        genome.fitness = train_genome((genome, config))
        print(f"📊 Genome {i+1}/{total_genomes} (ID: {genome_id}) achieved fitness: {genome.fitness}")

# Initialize the generation counter
eval_genome.current_generation = 0

def run_neat():
    """Run the NEAT algorithm."""
    # Use absolute path for config file
    config_path = os.path.join(SCRIPT_DIR, "config-feedforward.txt")
    
    # Load the config file
    config = neat.Config(neat.DefaultGenome, neat.DefaultReproduction,
                        neat.DefaultSpeciesSet, neat.DefaultStagnation,
                        config_path)
    
    # Create a new population or load existing population
    pop = neat.Population(config)
    
    # Add statistics reporter
    stats = neat.StatisticsReporter()
    pop.add_reporter(stats)
    
    # Add a reporter to show progress in the terminal
    pop.add_reporter(neat.StdOutReporter(True))
    
    # Add a custom reporter to track generations
    class GenerationReporter(neat.reporting.BaseReporter):
        def __init__(self):
            self.generation = 0
            self.best_fitness = 0
            
        def start_generation(self, generation):
            self.generation = generation
            eval_genome.current_generation = generation
            print(f"\n{'='*50}")
            print(f"🔄 Starting Generation {generation}")
            print(f"{'='*50}")
            
        def end_generation(self, config, population, species_set):
            print(f"\n{'='*50}")
            print(f"✅ Completed Generation {self.generation}")
            print(f"🏆 Best fitness: {self.best_fitness}")
            print(f"🧬 Number of species: {len(species_set.species)}")
            print(f"{'='*50}")
            
        def post_evaluate(self, config, population, species, best_genome):
            self.best_fitness = best_genome.fitness
            print(f"\n📈 Best fitness in Generation {self.generation}: {best_genome.fitness}")
            
            # Save the best genome of each generation
            save_dir = os.path.join(SCRIPT_DIR, "checkpoints")
            os.makedirs(save_dir, exist_ok=True)
            
            with open(os.path.join(save_dir, f"best_gen_{self.generation}.pkl"), "wb") as f:
                pickle.dump(best_genome, f)
                
            # Also save a checkpoint of the entire population every 5 generations
            if self.generation % 5 == 0:
                with open(os.path.join(save_dir, f"checkpoint_gen_{self.generation}.pkl"), "wb") as f:
                    pickle.dump(population, f)
    
    # Add the custom reporter
    pop.add_reporter(GenerationReporter())
    
    # Run for up to 50 generations
    winner = pop.run(eval_genome, 50)
    
    # Save with absolute path
    save_path = os.path.join(SCRIPT_DIR, "best_ai.pkl")
    with open(save_path, "wb") as f:
        pickle.dump(winner, f)
    
    print(f"\n🎉 Training complete! Best genome saved to {save_path}")
    print(f"🏆 Final best fitness: {winner.fitness}")

def check_admin():
    """ Check if the script is running as Administrator. If not, exit. """
    if not ctypes.windll.shell32.IsUserAnAdmin():
        print("❌ ERROR: This script must be run as Administrator!")
        print("🔴 Please restart the terminal in Administrator mode and try again.")
        print("🛑 Exiting program...")
        sys.exit(1)  # Exit the script immediately

def simple_preprocess(frame):
    """
    Basic preprocessing for a single frame when no previous frame is available.
    Resizes the frame and extracts basic features.
    """
    # Ensure frame is grayscale
    if len(frame.shape) > 2:
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    else:
        gray = frame
    
    # Resize to smaller dimensions (10x10) to reduce input size
    resized = cv2.resize(gray, (10, 40))
    
    # Apply edge detection to highlight obstacles and platforms
    edges = cv2.Canny(resized, 100, 200)
    
    # Normalize values to range [0, 1]
    normalized = edges.astype(np.float32) / 255.0
    
    # Flatten to 1D array for network input
    flattened = normalized.flatten()
    
    return flattened

if __name__ == "__main__":
    # ✅ Run the admin check before anything else
    check_admin()

    # ✅ Ensure the game can launch before starting AI training
    check_game_runs()

    # ✅ Start NEAT training if everything is good
    run_neat()
