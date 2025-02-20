# OpenGD Linux Troubleshooting Guide

## Diagnostic Tools

### System Information
```bash
# Graphics
glxinfo | grep "OpenGL"
lspci | grep -i vga

# Audio
pulseaudio --dump-conf
aplay -l

# Input
xinput list
evtest
```

## Common Problems and Solutions

### 1. Build Issues

#### CMake Configuration Fails
```bash
# Problem: Missing dependencies
# Solution:
sudo apt-get install build-essential cmake
sudo apt-get install libx11-dev libgl1-mesa-dev
```

#### Compilation Errors
- Check compiler version
- Verify all dependencies
- Clear build directory and retry

### 2. Runtime Issues

#### Game Crashes on Start
1. Check log files
2. Verify library dependencies
3. Check graphics driver compatibility

#### Performance Problems
1. Monitor system resources
2. Check for background processes
3. Verify graphics settings

### 3. Distribution-Specific Issues

#### Ubuntu/Debian
- Package conflicts
- Repository issues
- Version compatibility

#### Fedora
- SELinux configuration
- DNF repository issues
- Kernel module loading

#### Arch Linux
- Package versioning
- AUR dependencies
- System updates

## Advanced Troubleshooting

### Debug Mode
```bash
# Enable debug logging
export OPENGD_DEBUG=1
export OPENGD_LOG_LEVEL=debug

# Run with debug output
./opengd --debug
```

### Performance Profiling
```bash
# CPU profiling
perf record ./opengd
perf report

# Memory profiling
valgrind --tool=massif ./opengd
```

### System Logs
- Application logs: `~/.local/share/opengd/logs/`
- System logs: `/var/log/`
- X11 logs: `/var/log/Xorg.0.log` 