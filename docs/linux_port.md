# Linux Porting Guide for OpenGD

## Overview
OpenGD is currently designed to work with the AX game engine (based on Cocos2d-x). To make it Linux-compatible, we need to address several key areas including build system configuration, platform-specific code, and dependency management.

## Current Dependencies
- AX Game Engine
- OpenGL/Graphics APIs
- Audio Engine
- File System Operations
- Input Handling

## Current Progress

### 1. Build System Setup
- [x] Create CMake configuration for Linux
- [x] Set up proper library detection for Linux systems
- [x] Configure compiler flags for GCC/Clang
- [x] Set up pkg-config files for dependencies

### 2. Platform-Specific Code
- [x] Review and modify platform-dependent code in FileUtils
- [x] Update path handling to use Linux-style paths
- [x] Create platform abstraction layer
- [x] Modify window creation and management code
- [x] Update input handling for Linux events

### 3. Graphics and Rendering
- [x] Ensure OpenGL context creation works on Linux
- [ ] Test shader compatibility
- [x] Verify texture loading on Linux systems
- [x] Check for any DirectX-specific code that needs porting

### 4. Audio System
- [x] Port audio engine to Linux (using PulseAudio/ALSA)
- [x] Implement audio playback system
- [x] Add fallback support
- [ ] Test audio playback and effects
- [ ] Verify audio format compatibility

### 5. Dependencies
- [x] List all required Linux packages
- [ ] Create installation script for dependencies
- [x] Document system requirements
- [ ] Test with different Linux distributions

### 6. File System
- [ ] Update file path handling
- [ ] Modify save data location for Linux
- [ ] Ensure proper file permissions
- [ ] Handle case sensitivity issues

### 7. Input Handling
- [x] Implement Linux keyboard input
- [x] Add gamepad support for Linux
- [x] Map keyboard controls appropriately
- [x] Add input configuration options

### 8. Testing
- [ ] Test on multiple Linux distributions
- [ ] Performance testing
- [ ] Memory leak testing
- [ ] Graphics driver compatibility testing

## Required Changes

### File System Operations
Current file operations in BaseGameLayer.cpp need modification:

### Platform-Specific Code
The following files need review and updates:
- GameToolbox/getTextureString.h
- platform/FileUtils.h
- Audio engine implementation

## Build Instructions

1. Install required dependencies:
```bash
sudo apt-get install build-essential cmake libgl1-mesa-dev \
    libglu1-mesa-dev libasound2-dev libpulse-dev \
    libx11-dev libxrandr-dev libxi-dev libxinerama-dev \
    libxcursor-dev
```

2. Configure build:
```bash
mkdir build && cd build
cmake ..
```

3. Build:
```bash
make -j$(nproc)
```

## Known Issues to Address

1. File path separators (Windows vs Linux)
2. Audio engine compatibility
3. Graphics API differences
4. Input handling differences
5. Save file locations

## Testing Requirements

1. Test on major distributions:
   - Ubuntu LTS
   - Fedora
   - Arch Linux
   - Debian

2. Test with different graphics drivers:
   - NVIDIA
   - AMD
   - Intel

## Documentation Updates Needed

- [ ] Update build instructions for Linux
- [ ] Document Linux-specific configuration
- [ ] Add troubleshooting guide for Linux users
- [ ] Update system requirements

## Resources

- Linux Game Development Documentation
- OpenGL Linux Guidelines
- Audio Programming on Linux
- X11/Wayland Documentation

This porting guide will be updated as new issues and requirements are discovered during the porting process.

### Next Immediate Steps

1. **Platform Manager**
   - Created LinuxPlatformManager class
   - Implemented file system operations
   - Added Linux-specific path handling
   - Set up proper permissions management

2. **Build System**
   - Added pkg-config support
   - Created opengd.pc configuration
   - Updated CMake with Linux dependencies
   - Added installation targets

3. **Initial Compilation Test**
   - Test basic compilation on Ubuntu LTS
   - Focus on core engine components first
   - Verify library linkage
   - Document any compilation errors

### Progress Notes

1. **Completed Items**
   - Basic CMake configuration created
   - Platform abstraction layer structure defined
   - File system abstraction layer created

2. **In Progress**
   - Platform-specific implementations
   - Build system refinements
   - File system operations

3. **Known Issues**
   - Need to handle platform-specific paths (see BaseGameLayer.cpp):

### Next Immediate Steps

1. **Implement Linux Audio Engine**
   - Create audio backend interface
   - Implement PulseAudio support
   - Add ALSA fallback support
   - Test audio functionality

2. **Window Management**
   - Create X11 window handler
   - Set up OpenGL context
   - Implement window events
   - Handle resolution changes

3. **Input System**
   - Implement keyboard handling
   - Add gamepad support
   - Create input configuration
   - Test input latency

### Progress Notes

1. **Completed Items**
   - Basic CMake configuration created
   - Platform abstraction layer structure defined
   - File system abstraction layer created

2. **In Progress**
   - Platform-specific implementations
   - Build system refinements
   - File system operations

3. **Known Issues**
   - Need to handle platform-specific paths (see BaseGameLayer.cpp):

### Next Implementation Steps

1. **Input System Implementation**
   - Implement X11 keyboard handling
   - Add gamepad support
   - Create input mapping configuration
   - Test input responsiveness

2. **Audio System**
   - Create audio backend interface
   - Implement PulseAudio support
   - Add ALSA fallback support
   - Test audio functionality

### Recently Completed
1. **Audio System Implementation**
   - [x] Created LinuxAudioManager class
   - [x] Implemented PulseAudio support
   - [x] Added ALSA fallback support
   - [x] Created audio stream management
   - [x] Added volume control

2. **Build System Updates**
   - [x] Added PulseAudio dependencies
   - [x] Added ALSA dependencies
   - [x] Updated CMake configuration

3. **Input System**
   - [x] Created LinuxInputManager class
   - [x] Implemented X11 keyboard support
   - [x] Added gamepad support via joystick API
   - [x] Created input mapping system
   - [x] Added input state tracking

### Known Issues
1. **Audio System**
   - Need to test with various audio formats
   - Verify latency requirements
   - Test audio device switching
   - Implement proper error handling

### Next Steps
1. **Testing Phase**
   - Create comprehensive test suite
   - Test all implemented systems
   - Verify cross-distribution compatibility
   - Performance testing