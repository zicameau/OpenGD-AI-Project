#!/usr/bin/env python3

import os
import sys
import yaml
import subprocess
import platform
import shutil
from pathlib import Path
from analyze_cmake import CMakeAnalyzer, update_project_yml

class ProjectSetup:
    def __init__(self):
        self.project_root = os.path.dirname(os.path.abspath(__file__))
        
    def run_command(self, command, cwd=None):
        """Run a command and return its output"""
        try:
            if isinstance(command, list):
                cmd_str = ' '.join(command)
            else:
                cmd_str = command
            
            print(f"Running: {cmd_str}")
            
            result = subprocess.run(
                command,
                cwd=cwd or self.project_root,
                check=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                shell=isinstance(command, str)
            )
            return result.stdout
        except subprocess.CalledProcessError as e:
            print(f"Error during setup: {e}")
            sys.exit(1)

    def setup_dependencies(self):
        """Install system dependencies"""
        if sys.platform.startswith('linux'):
            # Load project configuration
            with open('project.yml', 'r') as f:
                config = yaml.safe_load(f)
            
            # Update package lists
            self.run_command(['sudo', 'apt-get', 'update'])
            
            # Install build and runtime dependencies
            deps = config['dependencies'].get('ubuntu', {})
            if deps:
                build_deps = deps.get('build', [])
                runtime_deps = deps.get('runtime', [])
                all_deps = build_deps + runtime_deps + ['libpulse-dev']
                
                if all_deps:
                    self.run_command(['sudo', 'apt-get', 'install', '-y'] + all_deps)

    def setup_directories(self):
        """Create necessary directories"""
        os.makedirs('build', exist_ok=True)
        os.makedirs('Content', exist_ok=True)

    def run(self):
        """Main setup process"""
        print("Setting up OpenGD-AI project...")
        self.setup_dependencies()
        self.setup_directories()

def main():
    setup = ProjectSetup()
    setup.run()

if __name__ == '__main__':
    main() 