#!/bin/bash

# Exit on first error
set -e

# Clean build directory first
rm -rf build
mkdir -p build
cd build

# Ensure compiler environment
export CC=/usr/bin/gcc
export CXX=/usr/bin/g++

# Build commands with maximum verbosity
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_VERBOSE_MAKEFILE=ON \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      .. --debug-output --trace-expand

make VERBOSE=1 -j$(nproc) || make VERBOSE=1 