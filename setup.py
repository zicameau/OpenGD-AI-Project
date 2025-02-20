#!/usr/bin/env python3

import os
import sys
import yaml
import subprocess
import platform
import shutil
from pathlib import Path

class ProjectSetup:
    def __init__(self):
        self.config = self.load_config()
        self.platform = self.detect_platform()
        self.project_root = Path.cwd()
    
    def load_config(self):
        with open('project.yml', 'r') as f:
            return yaml.safe_load(f)
    
    def detect_platform(self):
        system = platform.system().lower()
        if system == 'linux':
            # Detect Linux distribution
            try:
                with open('/etc/os-release') as f:
                    for line in f:
                        if line.startswith('ID='):
                            return line.split('=')[1].strip().strip('"')
            except FileNotFoundError:
                pass
        return system
    
    def run_command(self, cmd, check=True):
        print(f"Running: {' '.join(cmd)}")
        subprocess.run(cmd, check=check)
    
    def install_dependencies(self):
        if self.platform not in self.config['dependencies']:
            print(f"No dependencies defined for platform: {self.platform}")
            return
        
        deps = []
        for group in self.config['dependencies'][self.platform].values():
            deps.extend(group)
        
        if self.platform in ['ubuntu', 'debian']:
            self.run_command(['sudo', 'apt-get', 'update'])
            self.run_command(['sudo', 'apt-get', 'install', '-y'] + deps)
    
    def setup_engine(self):
        engine = self.config['engine']
        external_dir = self.project_root / 'external'
        engine_dir = external_dir / engine['name']
        
        external_dir.mkdir(exist_ok=True)
        
        if not engine_dir.exists():
            print(f"Cloning {engine['name']} engine...")
            self.run_command([
                'git', 'clone', 
                '-b', engine['branch'],
                engine['repository'],
                str(engine_dir)
            ])
        else:
            print(f"Updating {engine['name']} engine...")
            self.run_command(['git', 'pull'], cwd=engine_dir)
        
        return engine_dir
    
    def setup_cmake_modules(self, engine_dir):
        for module in self.config['cmake']['modules']:
            module_path = engine_dir / module['path']
            module_path.parent.mkdir(parents=True, exist_ok=True)
            
            with open(module_path, 'w') as f:
                f.write(module['content'])
    
    def setup_tools(self, engine_dir):
        for tool_name, tool_config in self.config['tools'].items():
            tool_path = engine_dir / tool_config['path']
            tool_path.parent.mkdir(parents=True, exist_ok=True)
            
            script_path = tool_path / tool_name
            with open(script_path, 'w') as f:
                f.write(tool_config['script'])
            
            script_path.chmod(0o755)
            
            # Add to PATH in .bashrc if not already present
            tool_dir = str(tool_path.absolute())
            bashrc_path = Path.home() / '.bashrc'
            
            with open(bashrc_path, 'r') as f:
                bashrc_content = f.read()
            
            path_export = f'export PATH="{tool_dir}:$PATH"'
            if path_export not in bashrc_content:
                with open(bashrc_path, 'a') as f:
                    f.write(f'\n{path_export}\n')
    
    def setup_build_dir(self):
        build_dir = self.project_root / 'build'
        build_dir.mkdir(exist_ok=True)
        
        # Clean build directory
        for item in build_dir.iterdir():
            if item.is_dir():
                shutil.rmtree(item)
            else:
                item.unlink()
    
    def setup(self):
        try:
            print(f"Setting up {self.config['name']} project...")
            
            # Install system dependencies
            self.install_dependencies()
            
            # Setup engine
            engine_dir = self.setup_engine()
            
            # Setup CMake modules
            self.setup_cmake_modules(engine_dir)
            
            # Setup tools
            self.setup_tools(engine_dir)
            
            # Setup build directory
            self.setup_build_dir()
            
            print("\nSetup complete!")
            print("You can now run: cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..")
            
        except Exception as e:
            print(f"Error during setup: {e}", file=sys.stderr)
            return 1
        
        return 0

if __name__ == '__main__':
    sys.exit(ProjectSetup().setup()) 