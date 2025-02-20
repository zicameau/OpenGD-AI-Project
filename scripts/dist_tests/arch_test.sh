#!/bin/bash

# Test script for Arch Linux

echo "Testing OpenGD on Arch Linux"

# Install dependencies
sudo pacman -Sy --noconfirm \
    base-devel \
    cmake \
    libx11 \
    mesa \
    pulseaudio \
    alsa-lib \
    libevdev \
    gtest

# Build and test
./scripts/run_tests.sh 