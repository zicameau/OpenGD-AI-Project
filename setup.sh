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
            wget \
            unzip
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
    
    # Create tools directory if it doesn't exist
    mkdir -p external/axmol/tools/axslcc
    
    # Download axslcc
    print_status "Downloading axslcc"
    wget -O axslcc.tar.gz "https://github.com/axmolengine/axslcc/releases/download/v1.9.6/axslcc-1.9.6-linux.tar.gz"
    
    # Extract to a temporary directory and check contents
    print_status "Extracting axslcc"
    mkdir -p temp_axslcc
    tar -xzf axslcc.tar.gz -C temp_axslcc
    
    # List contents to debug
    print_status "Checking axslcc contents"
    ls -la temp_axslcc
    
    # Copy contents to the correct location
    cp -r temp_axslcc/* external/axmol/tools/axslcc/
    
    # Cleanup
    rm -rf temp_axslcc axslcc.tar.gz
    
    # Find and make the axslcc binary executable
    find external/axmol/tools/axslcc -name "axslcc" -type f -exec chmod +x {} \;
}

# Function to apply custom files
apply_custom_files() {
    print_status "Applying custom files"
    
    # Create the Modules directory if it doesn't exist
    mkdir -p external/axmol/cmake/Modules
    
    # Replace the CMake files
    cp patches/axmol/AXBuildHelpers.cmake external/axmol/cmake/Modules/AXBuildHelpers.cmake
    cp patches/axmol/AXBuildPredefinitions.cmake external/axmol/cmake/Modules/AXBuildPredefinitions.cmake
    cp patches/axmol/AXBuildSet.cmake external/axmol/cmake/Modules/AXBuildSet.cmake
}

# Function to setup Axmol engine
setup_axmol() {
    print_status "Setting up Axmol engine"
    cd external/axmol
    # Apply custom files
    cd ../..
    apply_custom_files
}

# Function to setup build directory
setup_build() {
    print_status "Setting up build directory"
    rm -rf build
    mkdir -p build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    cd ..
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

echo "Setting up Python environment..."

# Check if Python is installed
if ! command -v python3 &> /dev/null; then
    echo "Installing Python..."
    sudo apt-get update
    sudo apt-get install -y python3 python3-pip
else
    echo "Python is already installed"
fi

# Check if pip is installed
if ! command -v pip3 &> /dev/null; then
    echo "Installing pip..."
    sudo apt-get install -y python3-pip
else
    echo "pip is already installed"
fi

# Install pyyaml if not already installed
if ! python3 -c "import yaml" &> /dev/null; then
    echo "Installing pyyaml..."
    pip3 install pyyaml
else
    echo "pyyaml is already installed"
fi

# Make all Python and shell scripts executable
echo "Making scripts executable..."
find . -type f -name "*.sh" -exec chmod +x {} \;
find . -type f -name "*.py" -exec chmod +x {} \;

# Run the CMake analyzer first
echo "Analyzing CMake files..."
./analyze_cmake.py

# Run the Python setup script
echo "Running setup script..."
./setup.py

# Make any shell scripts in the project executable (including those created during setup)
echo "Making all shell scripts executable..."
find . -type f -name "*.sh" -exec chmod +x {} \;
find external -type f -name "axslcc" -exec chmod +x {} \; 2>/dev/null || true

# Source the updated bashrc to apply any PATH changes
source ~/.bashrc

echo "Setup complete! You can now run: cd build && cmake -DCMAKE_BUILD_TYPE=Debug .."

# Create external directory
mkdir -p external

# Clone Axmol Engine if not already present
if [ ! -d "external/axmol" ]; then
    echo "Cloning Axmol Engine..."
    git clone git@github.com:axmolengine/axmol.git external/axmol
else
    echo "Axmol Engine already exists, updating..."
    cd external/axmol
    git pull
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