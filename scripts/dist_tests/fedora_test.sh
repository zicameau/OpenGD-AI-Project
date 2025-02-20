#!/bin/bash

# Test script for Fedora/RHEL-based distributions

echo "Testing OpenGD on Fedora"

# Install dependencies
sudo dnf install -y \
    gcc-c++ \
    cmake \
    libX11-devel \
    mesa-libGL-devel \
    pulseaudio-libs-devel \
    alsa-lib-devel \
    libevdev-devel \
    gtest-devel

# Build and test
./scripts/run_tests.sh 