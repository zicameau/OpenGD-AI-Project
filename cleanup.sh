#!/bin/bash

# Exit on any error
set -e

echo "Starting cleanup..."

# Restore CMake files if backups exist
if [ -f "external/axmol/cmake/Modules/AXSLCC.cmake.bak" ]; then
    echo "Restoring AXSLCC.cmake..."
    mv "external/axmol/cmake/Modules/AXSLCC.cmake.bak" "external/axmol/cmake/Modules/AXSLCC.cmake"
fi

if [ -f "external/axmol/cmake/Modules/AXBuildHelpers.cmake.bak" ]; then
    echo "Restoring AXBuildHelpers.cmake..."
    mv "external/axmol/cmake/Modules/AXBuildHelpers.cmake.bak" "external/axmol/cmake/Modules/AXBuildHelpers.cmake"
fi

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

echo "Cleanup complete!"
echo "You can now run setup.sh again for a fresh installation." 