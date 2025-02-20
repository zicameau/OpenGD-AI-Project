FROM archlinux:latest

# Update system and install dependencies
RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm \
    base-devel \
    cmake \
    git \
    libx11 \
    mesa \
    pulseaudio \
    alsa-lib \
    libevdev \
    gtest \
    && pacman -Scc --noconfirm

# Set working directory
WORKDIR /opengd

# Default command
CMD ["/bin/bash"] 