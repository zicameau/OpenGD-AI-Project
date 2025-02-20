# OpenGD Linux Installation Guide

## System Requirements

### Minimum Requirements
- CPU: Dual-core processor, 2.0 GHz
- RAM: 2GB
- Graphics: OpenGL 3.3 compatible GPU
- Storage: 500MB
- Sound: PulseAudio or ALSA compatible

### Recommended Requirements
- CPU: Quad-core processor, 2.5 GHz or better
- RAM: 4GB or more
- Graphics: OpenGL 4.0+ compatible GPU
- Storage: 1GB
- Sound: PulseAudio compatible

## Installation

### Ubuntu/Debian
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libx11-dev \
    libgl1-mesa-dev \
    libpulse-dev \
    libasound2-dev \
    libevdev-dev

# Build OpenGD
git clone https://github.com/OpenGD/OpenGD.git
cd OpenGD
mkdir build && cd build
cmake ..
make -j$(nproc)

# Install
sudo make install
```

### Fedora
```bash
# Install dependencies
sudo dnf install -y \
    gcc-c++ \
    cmake \
    libX11-devel \
    mesa-libGL-devel \
    pulseaudio-libs-devel \
    alsa-lib-devel \
    libevdev-devel

# Build and install steps same as Ubuntu
```

### Arch Linux
```bash
# Install dependencies
sudo pacman -S \
    base-devel \
    cmake \
    libx11 \
    mesa \
    pulseaudio \
    alsa-lib \
    libevdev

# Build and install steps same as Ubuntu
```

## Post-Installation

1. **Configure Audio**
   - Verify PulseAudio is running: `pulseaudio --check`
   - Test audio output: `speaker-test -c2`

2. **Configure Input**
   - Test keyboard input: Run OpenGD and verify controls
   - Configure gamepad (if applicable)

3. **Verify Graphics**
   - Check OpenGL version: `glxinfo | grep "OpenGL version"`
   - Verify vsync settings

## Common Issues and Solutions

### Audio Issues
1. **No Sound**
   - Check if PulseAudio is running
   - Verify audio device selection
   - Try ALSA fallback

2. **Audio Latency**
   - Adjust buffer size in PulseAudio
   - Check for system load issues

### Graphics Issues
1. **Poor Performance**
   - Update graphics drivers
   - Check OpenGL version
   - Verify compositor settings

2. **Screen Tearing**
   - Enable vsync
   - Check monitor refresh rate

### Input Issues
1. **Input Lag**
   - Check system load
   - Verify X11 configuration
   - Update input drivers

2. **Gamepad Not Detected**
   - Verify permissions
   - Check USB connection
   - Update gamepad firmware
```

## Performance Optimization

1. **Graphics**
   - Use compositor with OpenGL support
   - Enable hardware acceleration
   - Update graphics drivers

2. **Audio**
   - Use PulseAudio when possible
   - Optimize buffer sizes
   - Minimize audio processing

3. **Input**
   - Disable unnecessary input devices
   - Configure X11 input settings
   - Use gaming-optimized kernel (if available) 