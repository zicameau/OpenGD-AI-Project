#!/usr/bin/env python3

import os
import sys
import yaml
import shutil
from pathlib import Path

def cleanup():
    """Clean up build artifacts while preserving submodules"""
    # Directories to clean
    dirs_to_clean = [
        'build',
        '__pycache__',
        '.pytest_cache',
        'dist',
        'bin'
    ]
    
    # Files to clean
    extensions_to_clean = [
        '*.pyc',
        '*.pyo',
        '*.pyd',
        '*.so',
        '*.dll',
        '*.dylib',
        '*.log',
        '*.cache'
    ]
    
    project_root = Path.cwd()
    
    # Directories to preserve
    preserve_dirs = ['external/axmol', 'external/axslcc']
    
    # Clean directories
    for dir_name in dirs_to_clean:
        for path in project_root.rglob(dir_name):
            if not any(str(preserve) in str(path) for preserve in preserve_dirs):
                print(f"Removing directory: {path}")
                shutil.rmtree(path, ignore_errors=True)
    
    # Clean files
    for ext in extensions_to_clean:
        for path in project_root.rglob(ext):
            if not any(str(preserve) in str(path) for preserve in preserve_dirs):
                print(f"Removing file: {path}")
                path.unlink()

    try:
        # Load configuration
        with open('project.yml', 'r') as f:
            config = yaml.safe_load(f)
        
        # Remove external directory
        external_dir = project_root / 'external'
        if external_dir.exists():
            print("Removing external dependencies...")
            shutil.rmtree(external_dir)
        
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