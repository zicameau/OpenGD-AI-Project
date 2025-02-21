#!/usr/bin/env python3

import os
import re
import yaml

class CMakeAnalyzer:
    def __init__(self, root_dir='.'):
        self.root_dir = root_dir
        
    def find_cmake_files(self):
        """Find all CMake files in the directory tree"""
        cmake_files = []
        for root, _, files in os.walk(self.root_dir):
            for file in files:
                if file == "CMakeLists.txt" or file.endswith(".cmake"):
                    cmake_files.append(os.path.join(root, file))
        return cmake_files

    def extract_cmake_functions(self, content):
        """Extract CMake function names from content"""
        pattern = r'(?i)(?:^|\s)((?:ax_|[a-z_]+)\([^)]*\))'
        matches = re.findall(pattern, content)
        
        functions = set()
        for match in matches:
            func_name = match.split('(')[0].strip()
            functions.add(func_name)
        
        return sorted(list(functions))

    def create_stub_implementations(self, functions):
        """Create stub implementations for CMake functions"""
        stubs = {}
        for func in functions:
            stubs[func] = {
                'description': f'Stub implementation for {func}',
                'parameters': ['arg1', 'arg2'],
                'implementation': 'message(STATUS "Called ${CMAKE_CURRENT_FUNCTION}")'
            }
        return stubs

    def analyze(self):
        """Analyze CMake files and return found functions"""
        cmake_files = self.find_cmake_files()
        all_functions = set()
        
        for cmake_file in cmake_files:
            with open(cmake_file, 'r', encoding='utf-8') as f:
                try:
                    content = f.read()
                    functions = self.extract_cmake_functions(content)
                    all_functions.update(functions)
                except UnicodeDecodeError:
                    print(f"Warning: Could not read {cmake_file}")
        
        return sorted(list(all_functions))

def update_project_yml(functions):
    """Update project.yml with CMake function stubs"""
    analyzer = CMakeAnalyzer()
    stubs = analyzer.create_stub_implementations(functions)
    
    project_config = {
        'cmake_functions': stubs
    }
    
    with open('project.yml', 'w') as f:
        yaml.dump(project_config, f, default_flow_style=False)

def main():
    analyzer = CMakeAnalyzer()
    functions = analyzer.analyze()
    
    print("Found missing CMake functions:")
    for func in functions:
        print(f"  - {func}")
    
    update_project_yml(functions)
    print("\nUpdated project.yml with stub implementations")

if __name__ == '__main__':
    main() 