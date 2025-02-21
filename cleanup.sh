#!/bin/bash

# Exit on any error
set -e

echo "Starting cleanup..."

# Remove external directories
if [ -d "external" ]; then
    echo "Removing external dependencies..."
    rm -rf external
fi

# Remove build directory
if [ -d "build" ]; then
    echo "Removing build directory..."
    rm -rf build
fi

# Remove patches directory if it exists
if [ -d "patches" ]; then
    echo "Removing patches directory..."
    rm -rf patches
fi

# Clean up environment variables from .bashrc
echo "Cleaning up environment variables..."
sed -i '/export AX_ROOT=/d' ~/.bashrc
sed -i '/export PATH=.*axslcc/d' ~/.bashrc

# Source updated .bashrc
source ~/.bashrc

# Unset environment variables in current session
unset AX_ROOT

# Function to print status messages
print_status() {
    echo "===> $1"
}

# Clean build directory
print_status "Cleaning build directory"
rm -rf build/

# Clean Python cache
print_status "Cleaning Python cache"
find . -type d -name "__pycache__" ! -path "./external/axmol/*" ! -path "./external/axslcc/*" -exec rm -rf {} +
find . -type d -name ".pytest_cache" ! -path "./external/axmol/*" ! -path "./external/axslcc/*" -exec rm -rf {} +

# Clean compiled files
print_status "Cleaning compiled files"
find . -type f -name "*.pyc" ! -path "./external/axmol/*" ! -path "./external/axslcc/*" -delete
find . -type f -name "*.pyo" ! -path "./external/axmol/*" ! -path "./external/axslcc/*" -delete
find . -type f -name "*.pyd" ! -path "./external/axmol/*" ! -path "./external/axslcc/*" -delete
find . -type f -name "*.so" ! -path "./external/axmol/*" ! -path "./external/axslcc/*" -delete
find . -type f -name "*.dll" ! -path "./external/axmol/*" ! -path "./external/axslcc/*" -delete
find . -type f -name "*.dylib" ! -path "./external/axmol/*" ! -path "./external/axslcc/*" -delete

# Clean log files
print_status "Cleaning log files"
find . -type f -name "*.log" ! -path "./external/axmol/*" ! -path "./external/axslcc/*" -delete

echo "Cleanup complete!"
echo "You can now run setup.sh again for a fresh installation." 