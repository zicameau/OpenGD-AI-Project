#!/usr/bin/env python3

import os
import sys
import yaml
import shutil
from pathlib import Path

def cleanup():
    try:
        # Load configuration
        with open('project.yml', 'r') as f:
            config = yaml.safe_load(f)
        
        project_root = Path.cwd()
        
        # Remove external directory
        external_dir = project_root / 'external'
        if external_dir.exists():
            print("Removing external dependencies...")
            shutil.rmtree(external_dir)
        
        # Remove build directory
        build_dir = project_root / 'build'
        if build_dir.exists():
            print("Removing build directory...")
            shutil.rmtree(build_dir)
        
        # Clean up PATH additions in .bashrc
        bashrc_path = Path.home() / '.bashrc'
        if bashrc_path.exists():
            print("Cleaning up environment variables...")
            with open(bashrc_path, 'r') as f:
                lines = f.readlines()
            
            # Filter out our PATH additions
            new_lines = [
                line for line in lines
                if not any(
                    tool_name in line
                    for tool_name in config['tools'].keys()
                )
            ]
            
            with open(bashrc_path, 'w') as f:
                f.writelines(new_lines)
        
        print("Cleanup complete!")
        print("You can now run setup.py again for a fresh installation.")
        
    except Exception as e:
        print(f"Error during cleanup: {e}", file=sys.stderr)
        return 1
    
    return 0

if __name__ == '__main__':
    sys.exit(cleanup()) 