#!/usr/bin/env python3

import os
import re
from pathlib import Path

class CMakeAnalyzer:
    def __init__(self, root_dir):
        self.root_dir = Path(root_dir)
        self.function_calls = set()
        self.function_definitions = set()
        
    def find_cmake_files(self):
        """Find all CMake files in the project."""
        for path in self.root_dir.rglob('*'):
            if path.is_file() and (path.name == 'CMakeLists.txt' or path.suffix == '.cmake'):
                yield path
    
    def extract_function_calls(self, content):
        """Extract function calls from CMake content."""
        # Match function calls: function_name(args)
        pattern = r'([a-zA-Z_][a-zA-Z0-9_]*)\s*\('
        return set(re.findall(pattern, content))
    
    def extract_function_definitions(self, content):
        """Extract function definitions from CMake content."""
        # Match function definitions: function(name args)
        pattern = r'function\s*\(\s*([a-zA-Z_][a-zA-Z0-9_]*)'
        return set(re.findall(pattern, content))
    
    def analyze(self):
        """Analyze all CMake files in the project."""
        for cmake_file in self.find_cmake_files():
            with open(cmake_file, 'r', encoding='utf-8') as f:
                content = f.read()
                self.function_calls.update(self.extract_function_calls(content))
                self.function_definitions.update(self.extract_function_definitions(content))
        
        # Remove standard CMake commands
        standard_commands = {
            'project', 'cmake_minimum_required', 'add_executable', 'add_library',
            'target_link_libraries', 'set', 'include', 'if', 'endif', 'else',
            'elseif', 'while', 'endwhile', 'foreach', 'endforeach', 'function',
            'endfunction', 'macro', 'endmacro', 'add_subdirectory', 'find_package',
            'message', 'option', 'set_target_properties', 'target_include_directories',
            'add_custom_command', 'add_custom_target', 'file', 'list'
        }
        
        self.function_calls -= standard_commands
        
        # Find missing function definitions
        self.missing_functions = self.function_calls - self.function_definitions
        
        return self.missing_functions

def generate_cmake_module(missing_functions):
    """Generate CMake module content with stub functions."""
    content = []
    for func in sorted(missing_functions):
        content.extend([
            f"function({func})",
            "    cmake_parse_arguments(PARSE_ARGV 0 ARG \"\" \"\" \"\")",
            "    # TODO: Implement ${func}",
            "endfunction()",
            ""
        ])
    return "\n".join(content)

def update_project_yml(missing_functions):
    """Update project.yml with new CMake modules."""
    import yaml
    
    with open('project.yml', 'r') as f:
        config = yaml.safe_load(f)
    
    # Add or update the CustomFunctions module
    custom_functions_content = generate_cmake_module(missing_functions)
    
    # Find if CustomFunctions module already exists
    custom_module_exists = False
    for module in config['cmake']['modules']:
        if module['name'] == 'CustomFunctions':
            module['content'] = custom_functions_content
            custom_module_exists = True
            break
    
    if not custom_module_exists:
        config['cmake']['modules'].append({
            'name': 'CustomFunctions',
            'path': 'cmake/Modules/CustomFunctions.cmake',
            'content': custom_functions_content
        })
    
    # Update project.yml
    with open('project.yml', 'w') as f:
        yaml.dump(config, f, sort_keys=False)

def main():
    analyzer = CMakeAnalyzer('external/axmol')
    missing_functions = analyzer.analyze()
    
    if missing_functions:
        print("Found missing CMake functions:")
        for func in sorted(missing_functions):
            print(f"  - {func}")
        
        update_project_yml(missing_functions)
        print("\nUpdated project.yml with stub implementations")
    else:
        print("No missing CMake functions found")

if __name__ == '__main__':
    main() 