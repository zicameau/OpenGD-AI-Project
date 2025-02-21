#!/bin/bash

# Exit on any error
set -e

# Function to print status messages
print_status() {
    echo "===> $1"
}

# Function to check and install dependencies based on the system
install_dependencies() {
    if [ -f /etc/debian_version ]; then
        print_status "Detected Debian/Ubuntu system"
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            cmake \
            git \
            libx11-dev \
            libgl1-mesa-dev \
            libpulse-dev \
            libasound2-dev \
            libevdev-dev \
            libgtest-dev \
            python3-pip \
            nasm \
            libvlc-dev \
            libxrandr-dev \
            libxinerama-dev \
            libxcursor-dev \
            libxi-dev \
            libx11-6 \
            libgl1 \
            libasound2 \
            libevdev2 \
            libpulse0 \
            libvlc5
    else
        print_status "Unsupported system. Please install dependencies manually."
        exit 1
    fi
}

# Function to initialize and update submodules
setup_submodules() {
    print_status "Initializing and updating git submodules"
    git submodule update --init --recursive
}

# Function to setup axslcc
setup_axslcc() {
    print_status "Setting up axslcc"
    cd external/axslcc || exit 1
    
    print_status "Downloading axslcc"
    wget -O axslcc.tar.gz https://github.com/axmolengine/axslcc/releases/download/v1.9.6/axslcc-1.9.6-linux.tar.gz
    
    print_status "Extracting axslcc"
    tar xf axslcc.tar.gz
    rm axslcc.tar.gz
    
    print_status "Checking axslcc contents"
    ls -la
    
    cd ../..
}

# Function to create necessary directories
create_directories() {
    print_status "Creating necessary directories"
    mkdir -p build
    mkdir -p Content
}

# Function to apply custom files
apply_custom_files() {
    print_status "Applying custom files"
    
    # Create directories first
    create_directories
    
    # Replace the CMake files
    cp patches/axmol/AXBuildHelpers.cmake external/axmol/cmake/Modules/AXBuildHelpers.cmake
    cp patches/axmol/AXBuildPredefinitions.cmake external/axmol/cmake/Modules/AXBuildPredefinitions.cmake
    cp patches/axmol/AXBuildSet.cmake external/axmol/cmake/Modules/AXBuildSet.cmake
    cp patches/axmol/AXShaderHelpers.cmake external/axmol/cmake/Modules/AXShaderHelpers.cmake
    cp patches/axmol/AXOptionHelpers.cmake external/axmol/cmake/Modules/AXOptionHelpers.cmake
    cp patches/axmol/fetch.cmake external/axmol/cmake/Modules/AXFetchHelpers.cmake
    cp patches/axmol/fetch.cmake external/axmol/1k/fetch.cmake
    cp patches/axmol/zlib.cmake external/axmol/3rdparty/zlib/CMakeLists.txt
    cp patches/axmol/chipmunk.cmake external/axmol/3rdparty/chipmunk/src/CMakeLists.txt
    cp patches/axmol/chipmunk_root.cmake external/axmol/3rdparty/chipmunk/CMakeLists.txt
    cp patches/axmol/fmt.cmake external/axmol/3rdparty/fmt/CMakeLists.txt
    cp patches/axmol/core.cmake external/axmol/core/CMakeLists.txt
    cp patches/axmol/3rdparty.cmake external/axmol/3rdparty/CMakeLists.txt
    cp patches/axmol/freetype.cmake external/axmol/3rdparty/freetype/CMakeLists.txt
    cp patches/axmol/cpp_tests.cmake external/axmol/tests/cpp-tests/CMakeLists.txt
    
    # Remove problematic directories
    rm -rf external/axmol/3rdparty/chipmunk/demo
    
    # Create downloads directory
    mkdir -p "${PWD}/build/downloads"
    chmod -R 777 "${PWD}/build/downloads"
}

# Function to setup Axmol
setup_axmol() {
    print_status "Setting up Axmol engine"
    
    # Check if PowerShell is installed
    if ! command -v pwsh &> /dev/null; then
        print_status "Installing PowerShell..."
        # Download and install the Microsoft repository GPG key
        curl -O https://packages.microsoft.com/config/ubuntu/22.04/packages-microsoft-prod.deb
        sudo dpkg -i packages-microsoft-prod.deb
        rm packages-microsoft-prod.deb

        # Update package list and install PowerShell
        sudo apt-get update
        sudo apt-get install -y powershell
    fi

    # Apply all patches from patches/axmol directory
    print_status "Applying custom files"
    if [ -d "patches/axmol" ]; then
        for patch_file in patches/axmol/*; do
            if [ -f "$patch_file" ]; then
                filename=$(basename "$patch_file")
                target_path="external/axmol/cmake/Modules/$filename"
                print_status "Applying patch: $filename"
                cp "$patch_file" "$target_path"
            fi
        done
    fi
}

# Function to setup build directory
setup_build() {
    print_status "Setting up build directory"
    mkdir -p build
}

# Main execution
print_status "Starting setup process"

# Install system dependencies
install_dependencies

# Setup git submodules
setup_submodules

# Setup axslcc
setup_axslcc

# Setup Axmol
setup_axmol

# Setup build directory
setup_build

print_status "Setup completed successfully!"

# Setup Python environment
echo "Setting up Python environment..."

# Install Python dependencies
pip3 install pyyaml

# Make scripts executable
echo "Making scripts executable..."
find . -type f -name "*.sh" -exec chmod +x {} \;
find . -type f -name "*.py" -exec chmod +x {} \;

# Run the CMake analyzer
echo "Analyzing CMake files..."
if [ -f "./analyze_cmake.py" ]; then
    ./analyze_cmake.py
fi

# Run the Python setup script
echo "Running setup script..."
if [ -f "./setup.py" ]; then
    ./setup.py
fi

# Make shell scripts executable
echo "Making all shell scripts executable..."
find . -type f -name "*.sh" -exec chmod +x {} \;
find external -type f -name "axslcc" -exec chmod +x {} \; 2>/dev/null || true

# Source bashrc
source ~/.bashrc

echo "Setup complete! You can now build the project:"
echo "cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j\$(nproc)"

# Create external directory
mkdir -p external

# Clone or update Axmol Engine
if [ ! -d "external/axmol" ]; then
    echo "Cloning Axmol Engine..."
    git clone git@github.com:axmolengine/axmol.git external/axmol
    cd external/axmol
    git checkout dev
    cd ../..
else
    echo "Axmol Engine already exists, checking out dev branch..."
    cd external/axmol
    git fetch origin
    git checkout dev
    git reset --hard origin/dev
    cd ../..
fi

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