FROM ubuntu:latest

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libx11-dev \
    libgl1-mesa-dev \
    libpulse-dev \
    libasound2-dev \
    libevdev-dev \
    libgtest-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /opengd

# Default command (will be overridden by the test script)
CMD ["/bin/bash"] 