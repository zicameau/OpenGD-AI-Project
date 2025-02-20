# OpenGD Linux Development Guide

## Development Environment Setup

### Required Tools
- Git
- CMake (3.15+)
- GCC/Clang
- GDB/LLDB
- Visual Studio Code or other IDE

### Environment Setup
```bash
# Install development tools
# Ubuntu/Debian
sudo apt-get install git cmake build-essential gdb

# Fedora
sudo dnf install git cmake gcc-c++ gdb

# Arch Linux
sudo pacman -S git cmake base-devel gdb
```

## Building for Development

### Debug Build
```bash
mkdir build-debug && cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
```

### Release Build
```bash
mkdir build-release && cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

## Development Workflow

### 1. Code Style
- Follow existing code style
- Use clang-format
- Keep platform-specific code in platform/ directory

### 2. Testing
- Write unit tests for new features
- Run distribution tests
- Test on multiple configurations

### 3. Debugging
```bash
# Run with GDB
gdb ./opengd

# Common GDB commands
break main
run
bt
print variable
```

### 4. Performance Profiling
- Use perf for CPU profiling
- Use valgrind for memory analysis
- Monitor GPU performance

## Platform-Specific Considerations

### 1. File System
- Use PlatformManager for paths
- Handle case sensitivity
- Respect XDG specifications

### 2. Graphics
- Use OpenGL 3.3+ features
- Handle different GPU vendors
- Support various screen configurations

### 3. Input
- Support X11 input methods
- Handle multiple keyboard layouts
- Implement gamepad support

### 4. Audio
- Support both PulseAudio and ALSA
- Handle device changes
- Manage latency requirements

## Contributing Guidelines

### 1. Pull Requests
- Create feature branch
- Write clear commit messages
- Include test cases
- Update documentation

### 2. Code Review
- Follow review checklist
- Address feedback promptly
- Test on all supported distributions

### 3. Documentation
- Update relevant docs
- Add inline comments
- Include examples

## Debugging Tips

### 1. Common Issues
- X11 connection errors
- OpenGL context creation
- Audio device initialization
- Input device permissions

### 2. Useful Commands
```bash
# Debug X11
xwininfo
xprop

# Debug OpenGL
glxinfo
glxgears

# Debug Audio
pulseaudio --check
aplay -l

# Debug Input
xinput list
evtest
```

### 3. Log Files
- Application logs
- System logs
- X11 logs
- Audio logs

## Performance Optimization

### 1. Profiling Tools
```bash
# CPU Profiling
perf record ./opengd
perf report

# Memory Analysis
valgrind --tool=massif ./opengd
ms_print massif.out.*

# GPU Profiling
nvidia-smi
radeontop
```

### 2. Optimization Tips
- Minimize system calls
- Use efficient data structures
- Optimize render batches
- Reduce audio latency 