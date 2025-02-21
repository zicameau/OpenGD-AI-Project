#!/bin/bash

# Ensure compiler environment
export CC=/usr/bin/gcc
export CXX=/usr/bin/g++

# Build commands
cd build
cmake -DCMAKE_BUILD_TYPE=Debug .. --debug-output --trace-expand
make -j$(nproc) 