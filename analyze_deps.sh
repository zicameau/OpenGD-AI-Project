#!/bin/bash

# Exit on any error
set -e

echo "Analyzing CMake dependencies..."

# Function to extract find_package calls from CMake files
analyze_cmake_file() {
    local file="$1"
    echo "Analyzing $file..."
    
    # Find all find_package calls
    grep -h "find_package" "$file" 2>/dev/null | sort -u || true
    
    # Find all pkg_check_modules calls (commonly used for Linux packages)
    grep -h "pkg_check_modules" "$file" 2>/dev/null | sort -u || true
    
    # Find library checks
    grep -h "find_library" "$file" 2>/dev/null | sort -u || true
    
    # Find header checks
    grep -h "find_path" "$file" 2>/dev/null | sort -u || true
    
    # Find program checks
    grep -h "find_program" "$file" 2>/dev/null | sort -u || true
}

# Function to extract apt package names from CMake targets
map_cmake_to_apt() {
    local cmake_dep="$1"
    case "$cmake_dep" in
        *OpenGL*)
            echo "libgl1-mesa-dev"
            ;;
        *X11*)
            echo "libx11-dev"
            ;;
        *ALSA*)
            echo "libasound2-dev"
            ;;
        *ZLIB*)
            echo "zlib1g-dev"
            ;;
        *OpenSSL*)
            echo "libssl-dev"
            ;;
        *CURL*)
            echo "libcurl4-openssl-dev"
            ;;
        *Lua*)
            echo "liblua5.3-dev"
            ;;
        *fmt*)
            echo "libfmt-dev"
            ;;
        *GLEW*)
            echo "libglew-dev"
            ;;
        *GLFW*)
            echo "libglfw3-dev"
            ;;
        *VLC*)
            echo "libvlc-dev vlc"
            ;;
        *)
            echo "# Unknown dependency: $cmake_dep"
            ;;
    esac
}

# Create a temporary file for dependencies
DEPS_FILE=$(mktemp)

# Find all CMake files in the axmol directory
find external/axmol -name "*.cmake" -o -name "CMakeLists.txt" | while read -r file; do
    analyze_cmake_file "$file" >> "$DEPS_FILE"
done

# Process the dependencies
echo "Required system packages:"
echo "build-essential cmake nasm" # Base build requirements

# Extract unique package names and map them to apt packages
sort -u "$DEPS_FILE" | while read -r dep; do
    if [[ -n "$dep" ]]; then
        map_cmake_to_apt "$dep"
    fi
done | sort -u

# Clean up
rm "$DEPS_FILE" 