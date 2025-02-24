#!/bin/bash

# Print commands and exit on errors
set -xe

echo "Installing OpenGD Linux dependencies..."

# Detect package manager
if command -v apt-get >/dev/null 2>&1; then
    # Debian/Ubuntu
    sudo apt-get update
    sudo apt-get install -y \
        build-essential \
        cmake \
        libx11-dev \
        libxrandr-dev \
        libxinerama-dev \
        libxcursor-dev \
        libxi-dev \
        libgl-dev \
        libglu1-mesa-dev \
        libglfw3 \
        libglfw3-dev \
        libglew-dev \
        pkg-config
elif command -v dnf >/dev/null 2>&1; then
    # Fedora
    sudo dnf install -y \
        gcc-c++ \
        cmake \
        libX11-devel \
        libXrandr-devel \
        libXinerama-devel \
        libXcursor-devel \
        libXi-devel \
        mesa-libGL-devel \
        mesa-libGLU-devel \
        glfw-devel \
        glew-devel \
        pkgconfig
elif command -v pacman >/dev/null 2>&1; then
    # Arch Linux
    sudo pacman -Sy --noconfirm \
        base-devel \
        cmake \
        libx11 \
        libxrandr \
        libxinerama \
        libxcursor \
        libxi \
        mesa \
        glu \
        glfw-x11 \
        glew \
        pkg-config
else
    echo "Unsupported package manager. Please install dependencies manually:"
    echo "- CMake"
    echo "- X11 development files"
    echo "- OpenGL development files"
    echo "- GLFW3 development files"
    echo "- GLEW development files"
    exit 1
fi

echo "Dependencies installed successfully!"

# Verify installations
echo "Checking installations..."
pkg-config --cflags --libs x11
pkg-config --cflags --libs gl
pkg-config --cflags --libs glfw3

echo "All dependencies are installed and verified!" 