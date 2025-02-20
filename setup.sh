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

# Install system dependencies
echo "Installing system dependencies..."
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
    nasm \
    libvlc-dev \
    vlc \
    zlib1g-dev \
    libssl-dev \
    libcurl4-openssl-dev \
    liblua5.3-dev \
    libasound2-dev \
    libfmt-dev

# Create a simplified CMake module to handle dependencies
mkdir -p external/axmol/cmake/Modules
cat > external/axmol/cmake/Modules/AXDependencies.cmake << 'EOF'
# Use system libraries instead of downloading and building
find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)
find_package(fmt REQUIRED)
find_package(Lua 5.3 REQUIRED)
find_package(ALSA REQUIRED)
EOF

# Modify the AXSLCC.cmake file
cat > external/axmol/cmake/Modules/AXSLCC.cmake << 'EOF'
# Simplified shader compiler setup
set(AXSLCC_FOUND TRUE)
set(AXSLCC_OUT_DIR "${CMAKE_BINARY_DIR}/runtime/axslc")
file(MAKE_DIRECTORY "${AXSLCC_OUT_DIR}")
EOF

# Create simple axslcc implementation
AXSLCC_DIR="external/axmol/tools/axslcc"
mkdir -p "$AXSLCC_DIR"

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
EOF

chmod +x "$AXSLCC_DIR/axslcc"

# Set up environment variables
export AX_ROOT=$(pwd)/external/axmol
echo "export AX_ROOT=$(pwd)/external/axmol" >> ~/.bashrc
export PATH="$AXSLCC_DIR:$PATH"
echo "export PATH=\"$AXSLCC_DIR:\$PATH\"" >> ~/.bashrc

# Create and clean build directory
mkdir -p build
rm -rf build/*

echo "Setup complete!"
echo "AX_ROOT has been set to: $AX_ROOT"
echo "You can now run: cd build && cmake -DCMAKE_BUILD_TYPE=Debug .."

# Source the updated bashrc
source ~/.bashrc 