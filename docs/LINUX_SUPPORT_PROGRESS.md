# Linux Support Progress Checklist

## Display Handling
- [x] Fix display size detection
  - [x] Replace `glfwGetMonitorPhysicalSize` with `glfwGetVideoMode`
  - [x] Add proper error handling for mode detection
  - [x] Add debug logging for display resolution

## Resource Loading
- [x] Implement Linux resource handler
  - [x] Add environment variable configuration (OPENGD_RESOURCES)
  - [x] Add fallback default path
  - [x] Add user feedback label for resource search status
  - [x] Add debug logging for resource paths
  - [x] Add JSON validation for level data
  - [x] Add directory structure validation
  - [x] Improve error messages for missing files

## Build System Improvements
- [x] Update CMakeLists.txt with Linux dependencies
  - [x] Add X11 package finding
  - [x] Add OpenGL package finding
  - [x] Add GLFW3 package finding
  - [x] Configure Linux-specific include directories and libraries
- [x] Create dependency installation script
  - [x] Support for Debian/Ubuntu
  - [x] Support for Fedora
  - [x] Support for Arch Linux
  - [x] Add dependency verification

## Debug Improvements
- [x] Add resource loading debug logs
  - [x] Log search paths
  - [x] Log missing resource files
  - [x] Log display initialization status
  - [x] Log JSON parsing errors
  - [x] Log directory structure validation

## Known Issues
- [x] Empty Linux resource handler (Fixed)
- [x] Incorrect display size calculation (Fixed)
- [x] Silent resource loading failures (Fixed with debug logs)
- [x] No user feedback for resource loading status (Fixed)
- [x] JSON parsing errors for level data (Fixed)
- [x] Missing required directory structure (Fixed)
- [x] Level decompression failures (Fixed)

## Future Improvements
- [x] Add configuration for resource paths
- [x] Implement resource validation checks
- [x] Add detailed error reporting
- [ ] Create Linux-specific installation guide
- [ ] Add dependency installation script to build process
- [x] Create default level data structure
- [x] Add automatic Custom directory creation

## Level Loading Implementation

### Current Status
- [x] Implemented level loading from original Geometry Dash resources
- [x] Added support for loading levels from both Custom/mainLevels.json and directly from levels/*.txt files
- [x] Added error handling for corrupted level data
- [x] Fixed build errors related to missing headers
- [x] Added fallback to raw level data when decompression fails

### Issues Fixed
- [x] Fixed JSON parsing errors in ResourcesLoadingLayer
- [x] Added proper error handling for decompression failures
- [x] Fixed null pointer exception in string construction
- [x] Added detailed logging for debugging
- [x] Added missing include for GameToolbox/log.h in GJGameLevel.cpp
- [x] Fixed member variable name errors in GJGameLevel.cpp (_levelVersion → _gameVersion, _starRequested → _starsRequested)
- [x] Removed reference to non-existent _attempts member variable
- [x] Fixed deprecated implicit this capture warnings in CircleWave.cpp
- [x] Fixed segmentation fault when decompression fails

### Implementation Details
1. [x] Created a centralized LevelLoader utility to handle level loading consistently
2. [x] Modified ResourcesLoadingLayer to properly detect and use the levels directory
3. [x] Added robust error handling for compressed level data
4. [x] Improved logging to help diagnose issues
5. [x] Fixed build errors to ensure compatibility with Linux
6. [x] Added detection for uncompressed level data

### Next Steps
- [ ] Test with various level formats and compression types
- [ ] Add support for custom levels
- [ ] Improve error messages for users
- [ ] Address AX_PLATFORM_LINUX redefinition warning 