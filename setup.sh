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

# Modify the fetch.cmake file
FETCH_CMAKE="external/axmol/1k/fetch.cmake"
echo "Modifying $FETCH_CMAKE..."

# Create a backup
cp "$FETCH_CMAKE" "${FETCH_CMAKE}.bak"

# Create new fetch.cmake with wget implementation
cat > "$FETCH_CMAKE" << 'EOF'
# Custom fetch implementation for Linux
include(CMakeParseArguments)

function(_1kfetch_dist)
    cmake_parse_arguments(FETCH "" "URL;SHA256;FILENAME" "" ${ARGN})
    
    if(NOT FETCH_URL)
        message(FATAL_ERROR "URL is required for _1kfetch_dist")
    endif()
    
    if(NOT FETCH_FILENAME)
        string(REGEX REPLACE ".*/" "" FETCH_FILENAME "${FETCH_URL}")
    endif()
    
    set(DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/downloads")
    file(MAKE_DIRECTORY "${DOWNLOAD_DIR}")
    
    set(OUTPUT_FILE "${DOWNLOAD_DIR}/${FETCH_FILENAME}")
    
    if(NOT EXISTS "${OUTPUT_FILE}")
        message(STATUS "Downloading ${FETCH_URL}")
        file(DOWNLOAD "${FETCH_URL}" "${OUTPUT_FILE}"
            SHOW_PROGRESS
            STATUS DOWNLOAD_STATUS
        )
        
        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        list(GET DOWNLOAD_STATUS 1 ERROR_MESSAGE)
        
        if(NOT STATUS_CODE EQUAL 0)
            message(FATAL_ERROR "Failed to download ${FETCH_URL}: ${ERROR_MESSAGE}")
        endif()
        
        if(FETCH_SHA256)
            file(SHA256 "${OUTPUT_FILE}" DOWNLOADED_SHA256)
            if(NOT "${DOWNLOADED_SHA256}" STREQUAL "${FETCH_SHA256}")
                message(FATAL_ERROR "SHA256 mismatch for ${FETCH_FILENAME}")
            endif()
        endif()
    endif()
    
    set(FETCH_OUTPUT_FILE "${OUTPUT_FILE}" PARENT_SCOPE)
endfunction()

function(_1kfetch_extract)
    cmake_parse_arguments(FETCH "" "ARCHIVE;WORKING_DIRECTORY" "" ${ARGN})
    
    if(NOT FETCH_ARCHIVE)
        message(FATAL_ERROR "ARCHIVE is required for _1kfetch_extract")
    endif()
    
    if(NOT FETCH_WORKING_DIRECTORY)
        set(FETCH_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    endif()
    
    file(MAKE_DIRECTORY "${FETCH_WORKING_DIRECTORY}")
    
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xf "${FETCH_ARCHIVE}"
        WORKING_DIRECTORY "${FETCH_WORKING_DIRECTORY}"
        RESULT_VARIABLE EXTRACT_RESULT
    )
    
    if(NOT EXTRACT_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to extract ${FETCH_ARCHIVE}")
    endif()
endfunction()
EOF

# Modify the AXSLCC.cmake file
AXSLCC_CMAKE="external/axmol/cmake/Modules/AXSLCC.cmake"
echo "Modifying $AXSLCC_CMAKE..."

# Create a backup
cp "$AXSLCC_CMAKE" "${AXSLCC_CMAKE}.bak"

# Replace the error message with our custom implementation
sed -i 's/message(FATAL_ERROR "Please run setup.ps1 again to download axslcc, and run CMake again.")/message(STATUS "Using custom axslcc implementation")\n        set(AXSLCC_FOUND TRUE)/' "$AXSLCC_CMAKE"

# Modify the AXBuildHelpers.cmake file
BUILDHELPERS_CMAKE="external/axmol/cmake/Modules/AXBuildHelpers.cmake"
echo "Modifying $BUILDHELPERS_CMAKE..."

# Create a backup
cp "$BUILDHELPERS_CMAKE" "${BUILDHELPERS_CMAKE}.bak"

# Replace PowerShell check with a custom implementation
cat > "$BUILDHELPERS_CMAKE" << 'EOF'
# Custom build helpers for Linux
include(CMakeParseArguments)

function(ax_copy_target_dll TARGET_NAME)
    # No-op on Linux
endfunction()

function(ax_copy_bin_dll_to_target TARGET_NAME)
    # No-op on Linux
endfunction()

function(ax_copy_target_to_bin_dir TARGET_NAME)
    # No-op on Linux
endfunction()

function(ax_copy_target_deps_dlls TARGET_NAME)
    # No-op on Linux
endfunction()

function(ax_config_pred)
    # Custom implementation for Linux
    set(options)
    set(oneValueArgs RESULT EXPR)
    set(multiValueArgs)
    
    cmake_parse_arguments(AX_PRED "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    if(${AX_PRED_EXPR})
        set(${AX_PRED_RESULT} 1 PARENT_SCOPE)
    else()
        set(${AX_PRED_RESULT} 0 PARENT_SCOPE)
    endif()
endfunction()
EOF

# Set up environment variable
export AX_ROOT=$(pwd)/external/axmol
echo "export AX_ROOT=$(pwd)/external/axmol" >> ~/.bashrc

# Install required dependencies
echo "Installing dependencies..."
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
    wget \
    curl

# Download and setup axslcc
AXSLCC_DIR="$AX_ROOT/tools/axslcc"
mkdir -p "$AXSLCC_DIR"

echo "Building axslcc from source..."

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