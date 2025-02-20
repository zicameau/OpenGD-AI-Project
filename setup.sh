#!/bin/bash

# Exit on any error
set -e

# Create external directory
mkdir -p external

# Clone Axmol Engine
if [ ! -d "external/axmol" ]; then
    echo "Cloning Axmol Engine..."
    git clone https://github.com/axmolengine/axmol.git external/axmol
else
    echo "Axmol Engine already exists, updating..."
    cd external/axmol
    git pull
    cd ../..
fi

# Set up environment variable
export AX_ROOT=$(pwd)/external/axmol
echo "export AX_ROOT=$(pwd)/external/axmol" >> ~/.bashrc

# Download and setup axslcc
AXSLCC_DIR="$AX_ROOT/tools/axslcc"
if [ ! -d "$AXSLCC_DIR" ]; then
    echo "Downloading axslcc..."
    mkdir -p "$AXSLCC_DIR"
    
    # Determine system architecture
    ARCH=$(uname -m)
    if [ "$ARCH" = "x86_64" ]; then
        AXSLCC_URL="https://github.com/axmolengine/axmol-tools/releases/download/v1.0.0/axslcc-linux-x64.tar.gz"
    elif [ "$ARCH" = "aarch64" ]; then
        AXSLCC_URL="https://github.com/axmolengine/axmol-tools/releases/download/v1.0.0/axslcc-linux-arm64.tar.gz"
    else
        echo "Unsupported architecture: $ARCH"
        exit 1
    fi
    
    # Download and extract axslcc
    if ! wget "$AXSLCC_URL" -O axslcc.tar.gz; then
        echo "Failed to download axslcc from $AXSLCC_URL"
        echo "Attempting to build from source..."
        
        # Build axslcc from source
        git clone https://github.com/axmolengine/axmol-tools.git
        cd axmol-tools/axslcc
        mkdir build && cd build
        cmake ..
        make -j$(nproc)
        
        # Copy built binary to tools directory
        cp axslcc "$AXSLCC_DIR/"
        cd ../../..
        rm -rf axmol-tools
    else
        # Extract downloaded archive
        if ! tar -xzf axslcc.tar.gz -C "$AXSLCC_DIR"; then
            echo "Failed to extract axslcc"
            exit 1
        fi
        rm axslcc.tar.gz
    fi
    
    # Make axslcc executable
    chmod +x "$AXSLCC_DIR/axslcc"
fi

# Add axslcc to PATH
export PATH="$AXSLCC_DIR:$PATH"
echo "export PATH=\"$AXSLCC_DIR:\$PATH\"" >> ~/.bashrc

# Create build directory
mkdir -p build

# Verify installation
if [ -x "$AXSLCC_DIR/axslcc" ]; then
    echo "Setup complete!"
    echo "AX_ROOT has been set to: $AX_ROOT"
    echo "axslcc has been installed to: $AXSLCC_DIR"
    echo "You can now run: cd build && cmake -DCMAKE_BUILD_TYPE=Debug .."
else
    echo "Error: axslcc installation failed"
    exit 1
fi

# Source the updated bashrc
source ~/.bashrc 