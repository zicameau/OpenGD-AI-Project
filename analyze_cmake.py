#!/usr/bin/env python3

import os
import re
import yaml

def find_cmake_files(directory):
    """Find all CMake files in the directory tree"""
    cmake_files = []
    for root, _, files in os.walk(directory):
        for file in files:
            if file == "CMakeLists.txt" or file.endswith(".cmake"):
                cmake_files.append(os.path.join(root, file))
    return cmake_files

def extract_cmake_functions(content):
    """Extract CMake function names from content"""
    # Pattern to match CMake function calls
    pattern = r'(?i)(?:^|\s)((?:ax_|[a-z_]+)\([^)]*\))'
    matches = re.findall(pattern, content)
    
    # Extract function names without parameters
    functions = set()
    for match in matches:
        func_name = match.split('(')[0].strip()
        functions.add(func_name)
    
    return sorted(list(functions))

def create_stub_implementations(functions):
    """Create stub implementations for CMake functions"""
    stubs = {}
    for func in functions:
        stubs[func] = {
            'description': f'Stub implementation for {func}',
            'parameters': ['arg1', 'arg2'],
            'implementation': 'message(STATUS "Called ${CMAKE_CURRENT_FUNCTION}")'
        }
    return stubs

def main():
    # Find all CMake files
    cmake_files = find_cmake_files('.')
    
    # Extract all function calls
    all_functions = set()
    for cmake_file in cmake_files:
        with open(cmake_file, 'r', encoding='utf-8') as f:
            try:
                content = f.read()
                functions = extract_cmake_functions(content)
                all_functions.update(functions)
            except UnicodeDecodeError:
                print(f"Warning: Could not read {cmake_file}")
    
    print("Found missing CMake functions:")
    for func in sorted(all_functions):
        print(f"  - {func}")
    
    # Create and save stub implementations
    stubs = create_stub_implementations(all_functions)
    
    # Update project.yml
    project_config = {
        'cmake_functions': stubs
    }
    
    with open('project.yml', 'w') as f:
        yaml.dump(project_config, f, default_flow_style=False)
    
    print("\nUpdated project.yml with stub implementations")

if __name__ == '__main__':
    main() 