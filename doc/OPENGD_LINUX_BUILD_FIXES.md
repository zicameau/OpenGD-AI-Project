# Build System Fixes Checklist

## Critical Issues
- [x] Fix cmake_dependent_option error in core/CMakeLists.txt
  - Issue: Incorrect arguments for CMAKE_DEPENDENT_OPTION macro
  - File: external/axmol/core/CMakeLists.txt:60
  - Status: Fixed
  - Notes: 
    - Fixed syntax of cmake_dependent_option
    - Removed duplicate include
    - Added proper initialization of base options

- [✓] Fix OpenAL build errors
  - Issue: Cannot specify compile features for non-existent targets
  - File: external/axmol/3rdparty/openal/CMakeLists.txt
  - Status: Fixed
  - Notes: 
    - Added proper C++20 configuration
    - Added explicit language enables
    - Added Clang-specific compiler flags

- [ ] Fix X11 dependency errors
  - Issue: Missing X11 development files
  - Error: Could NOT find X11 (missing: X11_X11_INCLUDE_PATH X11_X11_LIB)
  - Status: New Issue
  - Notes:
    - Need to ensure X11 development packages are properly installed
    - May need additional X11-related packages

- [ ] Fix shader helper warnings
  - Issue: Invalid DEPENDS keyword in add_custom_command
  - File: external/axmol/cmake/Modules/AXShaderHelpers.cmake:53
  - Status: Pending
  - Notes:
    - Related to CMake policy CMP0175
    - Need to update shader compilation process

- [ ] Fix OpenAL compiler dialect error
  - Issue: Clang compiler not properly configured for C++ dialect
  - Error: "Target requires the language dialect 'CXX' but compiler does not support it"
  - Status: In Progress
  - Notes:
    - Clang using GCC's standard library instead of libc++
    - Added LLVM/Clang dependencies:
      - libc++-14-dev
      - libc++abi-14-dev
      - lld-14
    - Added compiler flags:
      ```cmake
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -nostdinc++")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
      ```
    - Added LLVM include paths:
      ```cmake
      include_directories(SYSTEM
          /usr/lib/llvm-14/include/c++/v1
          /usr/include/x86_64-linux-gnu
      )
      ```

## Important Issues
- [ ] Fix CMake project() ordering
  - Issue: project() should be called before enable_language()
  - Status: New Issue
  - Notes:
    - Need to reorganize CMake initialization order
    - Affects language and compiler detection

- [ ] Missing optional packages
  - [ ] webkit2gtk-4.1 or webkit2gtk-4.0
  - [ ] AudioIO
  - [ ] SoundIO
  - [ ] JACK
  - [ ] OpenSL
  - [ ] PortAudio
  - Status: Low priority - these are optional dependencies

- [ ] Update CMake minimum version in freetype
  - File: external/axmol/3rdparty/freetype/CMakeLists.txt:113
  - Issue: Deprecation warning for CMake < 3.10
  - Status: Low priority - still functional

## Moderate Issues
- [ ] Fix fmt library warnings
  - Issue: Feature cxx_std_11 unknown for CXX compiler
  - Status: In Progress
  - Notes:
    - Related to CMake project configuration
    - Will be resolved after fixing project initialization

## Completed Fixes
- [x] Create missing Resources directory
  - Path: /home/rowde/OpenGD-AI-Project/Resources
  - Status: Fixed
  - Notes: Added Resources directory creation to setup.sh

## Environment Details
- Build environment: Ubuntu/Debian
- CMake version: 3.31.5
- Compiler: Clang 14.0.0
- X11 Status: Packages installed but path resolution failing
- Current focus: X11 dependency resolution

## Next Steps
1. Implement X11 path fixes in CMakeLists.txt
2. Fix project initialization order
3. Verify X11 development files:
   ```bash
   ls -l /usr/include/X11
   ls -l /usr/lib/x86_64-linux-gnu/libX11.so
   pkg-config --cflags --libs x11
   ```
4. Consider adding explicit X11 configuration:
   ```cmake
   set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/usr/share/cmake/Modules/")
   set(X11_FIND_REQUIRED true)
   ```

## Notes
- Build system requires proper X11 development environment
- CMake project structure needs reorganization
- Consider adding debug output for X11 path resolution 