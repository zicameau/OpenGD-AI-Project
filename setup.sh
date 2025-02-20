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

# Create build directory
mkdir -p build

echo "Setup complete! AX_ROOT has been set to: $AX_ROOT"
echo "You can now run: cd build && cmake -DCMAKE_BUILD_TYPE=Debug .." 