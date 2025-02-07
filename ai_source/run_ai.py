import json
import os
import pickle
import neat
import numpy as np
import cv2
import mss
import pyautogui
import time
import subprocess

# Load config file
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CONFIG_PATH = os.path.join(SCRIPT_DIR, "config.json")

with open(CONFIG_PATH, "r") as f:
    config = json.load(f)

# Resolve game path
GAME_PATH = os.path.abspath(os.path.join(SCRIPT_DIR, "..", config["game_path"]))

# Load trained AI model
with open("best_ai.pkl", "rb") as f:
    best_genome = pickle.load(f)

config_path = "config-feedforward.txt"
config = neat.Config(neat.DefaultGenome, neat.DefaultReproduction,
                     neat.DefaultSpeciesSet, neat.DefaultStagnation,
                     config_path)
net = neat.nn.FeedForwardNetwork.create(best_genome, config)

# Start the game
def start_game():
    print(f"Starting OpenGD from {GAME_PATH}...")
    process = subprocess.Popen(GAME_PATH, shell=True)
    time.sleep(5)
    return process

# Capture game screen
def capture_screen():
    with mss.mss() as sct:
        screenshot = sct.grab({"top": 100, "left": 100, "width": 800, "height": 600})
        img = np.array(screenshot)
        return cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

# AI plays the game
def run_ai():
    start_game()
    while True:
        frame = capture_screen()
        inputs = frame.flatten()[:10]
        output = net.activate(inputs)

        if output[0] > 0.5:
            pyautogui.press("space")  

        time.sleep(0.05)

if __name__ == "__main__":
    run_ai()
