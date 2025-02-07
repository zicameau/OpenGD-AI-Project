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

def start_game():
    """Attempts to start OpenGD and verifies that it is running."""
    for process in psutil.process_iter(attrs=['pid', 'name']):
        if "OpenGD.exe" in process.info['name']:
            print("✅ OpenGD is already running.")
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

        # Check if OpenGD is running
        for process in psutil.process_iter(attrs=['pid', 'name']):
            if "OpenGD.exe" in process.info['name']:
                print("✅ OpenGD is running!")
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
            if win32gui.IsWindowVisible(hwnd) and "OpenGD" in win32gui.GetWindowText(hwnd):
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
    """Capture the OpenGD window and return a grayscale image."""
    try:
        hwnd = get_window_handle()
        if not hwnd:
            return None
            
        if IS_WINDOWS:
            rect = win32gui.GetWindowRect(hwnd)
            x = rect[0]
            y = rect[1]
            width = rect[2] - x
            height = rect[3] - y
        else:
            geom = hwnd.get_geometry()
            x = geom.x
            y = geom.y
            width = geom.width
            height = geom.height
        
        # Capture the window
        with mss.mss() as sct:
            screenshot = sct.grab({
                "top": y,
                "left": x,
                "width": width,
                "height": height
            })
            
            img = np.array(screenshot)
            gray_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

            # Create named window and move it
            cv2.namedWindow("AI View", cv2.WINDOW_NORMAL)
            cv2.moveWindow("AI View", x + width + 10, y)
            cv2.imshow("AI View", gray_img)
            cv2.waitKey(1)

            return gray_img
        
    except Exception as e:
        print(f"❌ ERROR: {str(e)}")
        return None

def jump():
    """Simulate spacebar press."""
    if IS_WINDOWS:
        pyautogui.press('space')
    else:
        subprocess.run(['xdotool', 'key', 'space'])

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
    fitness = 0
    start_time = time.time()
    successful_jumps = 0
    prev_frame = None

    try:
        # Click both buttons in sequence
        click_play_button()
        click_level_button()
        
        while True:
            frame = capture_screen()
            if frame is None:
                break
                
            # Detect death by checking for large changes in the center region
            if prev_frame is not None:
                # Get the center region of the frame
                h, w = frame.shape
                center_y = h // 2
                center_x = w // 2
                region_size = 200
                
                current_region = frame[
                    center_y - region_size:center_y + region_size,
                    center_x - region_size:center_x + region_size
                ]
                prev_region = prev_frame[
                    center_y - region_size:center_y + region_size,
                    center_x - region_size:center_x + region_size
                ]
                
                # Calculate difference in center region
                frame_diff = np.mean(np.abs(current_region.astype(np.float32) - prev_region.astype(np.float32)))
                print(f"Frame diff: {frame_diff}")
                if frame_diff > 6:  # Lowered threshold based on observed values
                    print("💀 AI Died! Closing game...")
                    close_game()
                    break

            prev_frame = frame.copy()
            
            # Reduce the input size to match the network's expected input
            resized_frame = cv2.resize(frame, (2, 5))  # 2x5 = 10 inputs
            inputs = resized_frame.flatten() / 255.0  # Normalize pixel values
            output = net.activate(inputs)

            if output[0] > 0.5:
                jump()
                successful_jumps += 1
                print("🚀 AI Jumped!")

            fitness += 1 + (successful_jumps * 5)
            
            time.sleep(0.01)
            
            if time.time() - start_time > 30:
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
    for genome_id, genome in genomes:
        if not start_game():
            print("❌ ERROR: OpenGD did not start properly. Skipping genome evaluation.")
            genome.fitness = 0
            continue

        genome.fitness = train_genome((genome, config))

def run_neat():
    """Run the NEAT algorithm."""
    # Use absolute path for config file
    config_path = os.path.join(SCRIPT_DIR, "config-feedforward.txt")
    config = neat.Config(neat.DefaultGenome, neat.DefaultReproduction,
                        neat.DefaultSpeciesSet, neat.DefaultStagnation,
                        config_path)

    pop = neat.Population(config)
    
    # Add statistics reporter
    stats = neat.StatisticsReporter()
    pop.add_reporter(stats)
    pop.add_reporter(neat.StdOutReporter(True))
    
    # Run for up to 50 generations
    winner = pop.run(eval_genome, 50)
    
    # Save with absolute path
    save_path = os.path.join(SCRIPT_DIR, "best_ai.pkl")
    with open(save_path, "wb") as f:
        pickle.dump(winner, f)

def check_admin():
    """ Check if the script is running as Administrator. If not, exit. """
    if not ctypes.windll.shell32.IsUserAnAdmin():
        print("❌ ERROR: This script must be run as Administrator!")
        print("🔴 Please restart the terminal in Administrator mode and try again.")
        print("🛑 Exiting program...")
        sys.exit(1)  # Exit the script immediately


if __name__ == "__main__":
    # ✅ Run the admin check before anything else
    check_admin()

    # ✅ Ensure the game can launch before starting AI training
    check_game_runs()

    # ✅ Start NEAT training if everything is good
    run_neat()
