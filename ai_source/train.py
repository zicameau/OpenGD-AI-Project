import neat
import json
import os
import numpy as np
import cv2
import mss
import pyautogui
import time
import subprocess
import multiprocessing
import pickle
import psutil

# Load config file
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CONFIG_PATH = os.path.join(SCRIPT_DIR, "config.json")

with open(CONFIG_PATH, "r") as f:
    config = json.load(f)

# Resolve game path
GAME_PATH = os.path.abspath(os.path.join(SCRIPT_DIR, "..", config["game_path"]))

# Start the game if not running
def start_game():
    for process in psutil.process_iter(attrs=['pid', 'name']):
        if "OpenGD.exe" in process.info['name']:
            print("Game is already running.")
            return None
    print(f"Starting OpenGD from {GAME_PATH}...")
    process = subprocess.Popen(GAME_PATH, shell=True)
    time.sleep(5)
    return process

# Capture screen
def capture_screen():
    with mss.mss() as sct:
        screenshot = sct.grab({"top": 100, "left": 100, "width": 800, "height": 600})
        img = np.array(screenshot)
        return cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

# Simulate AI jump
def jump():
    pyautogui.press("space")

# Evaluate AI fitness
def eval_genome(genomes, config):
    game_process = start_game()
    if not game_process:
        return  

    def train_genome(genome):
        net = neat.nn.FeedForwardNetwork.create(genome, config)
        fitness = 0
        start_time = time.time()

        while True:
            frame = capture_screen()
            inputs = frame.flatten()[:10]  
            output = net.activate(inputs)

            if output[0] > 0.5:
                jump()

            fitness += 1  
            if time.time() - start_time > 10:
                break

        return fitness

    with multiprocessing.Pool(processes=10) as pool:
        results = pool.map(train_genome, [genome for _, genome in genomes])
        for (_, genome), fitness in zip(genomes, results):
            genome.fitness = fitness

# Train AI
def run_neat():
    config_path = "config-feedforward.txt"
    config = neat.Config(neat.DefaultGenome, neat.DefaultReproduction,
                         neat.DefaultSpeciesSet, neat.DefaultStagnation,
                         config_path)

    pop = neat.Population(config)
    pop.run(eval_genome, 50)  

    with open("best_ai.pkl", "wb") as f:
        pickle.dump(pop.best_genome, f)

if __name__ == "__main__":
    run_neat()
