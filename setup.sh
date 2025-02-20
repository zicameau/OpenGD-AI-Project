#!/bin/bash

# Exit on any error
set -e

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

# Analyze dependencies
echo "Analyzing dependencies..."
./analyze_deps.sh > dependencies.txt

# Install system dependencies
echo "Installing system dependencies..."
sudo apt-get update
sudo apt-get install -y $(cat dependencies.txt | grep -v '^#')

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