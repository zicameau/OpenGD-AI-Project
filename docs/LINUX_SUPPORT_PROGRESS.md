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
- [ ] JSON parsing errors for level data
- [ ] Missing required directory structure

## Future Improvements
- [x] Add configuration for resource paths
- [x] Implement resource validation checks
- [ ] Add detailed error reporting
- [ ] Create Linux-specific installation guide
- [ ] Add dependency installation script to build process
- [ ] Create default level data structure
- [ ] Add automatic Custom directory creation 