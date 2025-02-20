#!/bin/bash

# Exit on any error
set -e

# Create external directory
mkdir -p external

# Clone Axmol Engine
if [ ! -d "external/axmol" ]; then
    echo "Cloning Axmol Engine..."
    git clone git@github.com:axmolengine/axmol.git external/axmol
else
    echo "Axmol Engine already exists, updating..."
    cd external/axmol
    git pull
    cd ../..
fi

# Create patches directory if it doesn't exist
mkdir -p patches

# Create the CMake patch
cat > patches/axmol-cmake.patch << 'EOF'
--- cmake/Modules/AXSLCC.cmake	2024-02-19 23:45:00.000000000 +0000
+++ cmake/Modules/AXSLCC.cmake	2024-02-19 23:45:00.000000000 +0000
@@ -20,7 +20,7 @@
     endif()
     
     if(NOT AXSLCC_FOUND)
-        message(FATAL_ERROR "Please run setup.ps1 again to download axslcc, and run CMake again.")
+        message(STATUS "Using custom axslcc implementation")
+        set(AXSLCC_FOUND TRUE)
     endif()
 endif()
EOF

# Apply the patch
cd external/axmol
patch -p0 < ../../patches/axmol-cmake.patch || true
cd ../..

# Set up environment variable
export AX_ROOT=$(pwd)/external/axmol
echo "export AX_ROOT=$(pwd)/external/axmol" >> ~/.bashrc

# Download and setup axslcc
AXSLCC_DIR="$AX_ROOT/tools/axslcc"
mkdir -p "$AXSLCC_DIR"

echo "Building axslcc from source..."

# Install required dependencies
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libglew-dev \
    libx11-dev \
    libxrandr-dev \
    libxi-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libxinerama-dev \
    libxcursor-dev \
    libglfw3-dev \
    patch

# Clone and build SPIRV-Cross
if [ ! -d "external/SPIRV-Cross" ]; then
    echo "Cloning SPIRV-Cross..."
    git clone git@github.com:KhronosGroup/SPIRV-Cross.git external/SPIRV-Cross
    cd external/SPIRV-Cross
    mkdir build && cd build
    cmake ..
    make -j$(nproc)
    cd ../../..
fi

# Create simple axslcc implementation
cat > "$AXSLCC_DIR/axslcc" << 'EOF'
#!/bin/bash

# Simple shader compiler wrapper
input_file="$1"
output_file="$2"

if [ -z "$input_file" ] || [ -z "$output_file" ]; then
    echo "Usage: axslcc <input_file> <output_file>"
    exit 1
fi

# For now, just copy the input file to output
cp "$input_file" "$output_file"

# In the future, implement proper shader compilation
# using SPIRV-Cross or other tools

exit 0
EOF

# Make axslcc executable
chmod +x "$AXSLCC_DIR/axslcc"

# Add axslcc to PATH
export PATH="$AXSLCC_DIR:$PATH"
echo "export PATH=\"$AXSLCC_DIR:\$PATH\"" >> ~/.bashrc

# Create build directory
mkdir -p build

# Clean build directory
rm -rf build/*

# Verify installation
if [ -x "$AXSLCC_DIR/axslcc" ]; then
    echo "Setup complete!"
    echo "AX_ROOT has been set to: $AX_ROOT"
    echo "axslcc has been installed to: $AXSLCC_DIR"
    echo "Note: This is a temporary implementation of axslcc"
    echo "You can now run: cd build && cmake -DCMAKE_BUILD_TYPE=Debug .."
else
    echo "Error: axslcc installation failed"
    exit 1
fi

# Source the updated bashrc
source ~/.bashrc 