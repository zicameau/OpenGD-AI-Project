# Linux Support Progress Checklist

## Display Handling
- [x] Fix display size detection
  - [x] Replace `glfwGetMonitorPhysicalSize` with `glfwGetVideoMode`
  - [x] Add proper error handling for mode detection
  - [x] Add debug logging for display resolution

## Resource Loading
- [x] Implement Linux resource handler
  - [x] Add common Steam paths search:
    - `~/.steam/steam/steamapps/common/Geometry Dash/`
    - `~/.local/share/Steam/steamapps/common/Geometry Dash/`
  - [x] Add user feedback label for resource search status
  - [ ] Implement manual path specification fallback

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
  - [x] Log missing resource files:
    - `game_bg_01_001-hd.png`
    - `GJ_LaunchSheet-hd.png`
    - `GJ_GameSheet03-uhd.png`
  - [x] Log display initialization status

## Known Issues
- [x] Empty Linux resource handler (Fixed)
- [x] Incorrect display size calculation (Fixed)
- [x] Silent resource loading failures (Fixed with debug logs)
- [ ] No user feedback for resource loading status

## Future Improvements
- [ ] Add configuration file for custom resource paths
- [ ] Implement resource validation checks
- [ ] Add detailed error reporting
- [ ] Create Linux-specific installation guide
- [ ] Add dependency installation script to build process
- [ ] Implement automatic resource path detection from Steam library 