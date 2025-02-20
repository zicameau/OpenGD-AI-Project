#!/bin/bash

# Test script for Ubuntu/Debian-based distributions

echo "Testing OpenGD on Ubuntu/Debian"

# Install dependencies
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libx11-dev \
    libgl1-mesa-dev \
    libpulse-dev \
    libasound2-dev \
    libevdev-dev \
    libgtest-dev

# Build and test
./scripts/run_tests.sh 