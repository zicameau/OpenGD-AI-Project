#!/bin/bash

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
        AXSLCC_URL="https://github.com/axmolengine/axmol/releases/download/v2.0.0/axslcc-linux-x64.tar.gz"
    elif [ "$ARCH" = "aarch64" ]; then
        AXSLCC_URL="https://github.com/axmolengine/axmol/releases/download/v2.0.0/axslcc-linux-arm64.tar.gz"
    else
        echo "Unsupported architecture: $ARCH"
        exit 1
    fi
    
    # Download and extract axslcc
    wget "$AXSLCC_URL" -O axslcc.tar.gz
    tar -xzf axslcc.tar.gz -C "$AXSLCC_DIR"
    rm axslcc.tar.gz
    
    # Make axslcc executable
    chmod +x "$AXSLCC_DIR/axslcc"
fi

# Add axslcc to PATH
export PATH="$AXSLCC_DIR:$PATH"
echo "export PATH=\"$AXSLCC_DIR:\$PATH\"" >> ~/.bashrc

# Create build directory
mkdir -p build

echo "Setup complete!"
echo "AX_ROOT has been set to: $AX_ROOT"
echo "axslcc has been installed to: $AXSLCC_DIR"
echo "You can now run: cd build && cmake -DCMAKE_BUILD_TYPE=Debug .."

# Source the updated bashrc
source ~/.bashrc 