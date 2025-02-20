FROM fedora:latest

# Install dependencies
RUN dnf install -y \
    gcc-c++ \
    cmake \
    git \
    libX11-devel \
    mesa-libGL-devel \
    pulseaudio-libs-devel \
    alsa-lib-devel \
    libevdev-devel \
    gtest-devel \
    && dnf clean all

# Set working directory
WORKDIR /opengd

# Default command
CMD ["/bin/bash"] 