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
- [x] JSON parsing errors for raw level data (Fixed)
- [x] Missing function declarations in BaseGameLayer (Fixed)

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
- [x] Added format detection for XML, JSON, and compressed level data
- [x] Added proper function declarations in header files
- [x] Fixed ZipUtils usage for proper base64 decoding and decompression
- [x] Added detailed debugging logs to diagnose level loading issues

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
- [x] Added XML parsing fallback for level data
- [x] Fixed missing function declarations in BaseGameLayer.h
- [x] Fixed double compression issue with level data
- [x] Improved error handling in level loading process

### Implementation Details
1. [x] Created LevelLoader namespace with helper functions
2. [x] Implemented level string decompression
3. [x] Added support for different level formats (compressed, XML, JSON)
4. [x] Improved logging to help diagnose issues
5. [x] Fixed build errors to ensure compatibility with Linux
6. [x] Added detection for uncompressed level data
7. [x] Added format-specific parsing for XML and JSON level data
8. [x] Added proper header declarations for all new functions
9. [x] Simplified level loading process to avoid redundant operations
10. [x] Fixed ZipUtils API usage for proper decompression

### Next Steps
- [ ] Test with various level formats and compression types
- [ ] Add support for custom levels
- [ ] Improve error messages for users
- [ ] Address AX_PLATFORM_LINUX redefinition warning 