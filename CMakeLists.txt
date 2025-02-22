#/****************************************************************************
# Copyright (c) 2013-2014 cocos2d-x.org
# Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
#
# https://axmol.dev/
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

# Add at the top of the file, before any other compiler settings
set(CMAKE_C_COMPILER /usr/bin/clang-14)
set(CMAKE_CXX_COMPILER /usr/bin/clang++-14)

# Force CMake to use Clang
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)

# Set C++ standard flags explicitly for Clang
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add compiler feature flags
add_compile_options(-stdlib=libc++)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

# Set X11 paths explicitly
set(X11_X11_INCLUDE_PATH "/usr/include/X11")
set(X11_X11_LIB "/usr/lib/x86_64-linux-gnu/libX11.so")

# Additional X11 hints
set(X11_ROOT_DIR "/usr")
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};/usr/lib/x86_64-linux-gnu")

# Make sure pkg-config can find X11
set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/lib/x86_64-linux-gnu/pkgconfig")

# Debug output
message(STATUS "X11_X11_INCLUDE_PATH: ${X11_X11_INCLUDE_PATH}")
message(STATUS "X11_X11_LIB: ${X11_X11_LIB}")

# Enable language support
enable_language(CXX)

if(POLICY CMP0175)
    cmake_policy(SET CMP0175 NEW)
endif()

cmake_minimum_required(VERSION 3.20)

# Initialize project first
project(OpenGD_Game VERSION 1.0.0 LANGUAGES C CXX)

# Add after project() declaration but before other compiler settings
message(STATUS "C++ Compiler details:")
message(STATUS "  Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Compiler path: ${CMAKE_CXX_COMPILER}")
message(STATUS "  Compiler flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")

# Force compiler settings before any targets
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
    
    # Force these settings to propagate to all targets
    add_compile_options(-std=c++20 -stdlib=libc++)
    add_link_options(-stdlib=libc++)
endif()

# Add after the compiler selection but before project()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Force LLVM/Clang toolchain
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -nostdinc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
    
    # Add LLVM include paths
    include_directories(SYSTEM
        /usr/lib/llvm-14/include/c++/v1
        /usr/include/x86_64-linux-gnu
    )
    
    # Add LLVM library paths
    link_directories(
        /usr/lib/llvm-14/lib
        /usr/lib/x86_64-linux-gnu
    )
endif()

# Configure X11 with pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(PC_X11 REQUIRED x11)

# Set X11 paths using pkg-config results
set(X11_X11_INCLUDE_PATH ${PC_X11_INCLUDE_DIRS})
set(X11_X11_LIB ${PC_X11_LIBRARIES})

# Additional X11 configuration
set(X11_FIND_REQUIRED TRUE)
find_package(X11 REQUIRED)

if(NOT X11_FOUND)
    message(FATAL_ERROR "X11 not found. Paths tried:\n"
            "  Include: ${X11_X11_INCLUDE_PATH}\n"
            "  Library: ${X11_X11_LIB}")
endif()

# Set C++20 globally
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
endif()

# Add these near the top, after project() declaration
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ compiler version: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ compiler ID: ${CMAKE_CXX_COMPILER_ID}")

# Set Axmol root directory
set(_AX_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/axmol")

# Disable tests and demos
set(AX_BUILD_TESTS OFF CACHE BOOL "Build cpp-tests" FORCE)
set(AX_BUILD_UNIT_TESTS OFF CACHE BOOL "Build unit-tests" FORCE)
set(AX_BUILD_SAMPLES OFF CACHE BOOL "Build samples" FORCE)

# Configure Axmol build options
option(AX_USE_COMPAT_GL "Use OpenGL compatibility profile" OFF)
option(AX_USE_CURL "Use libcurl for HttpClient" ON)
option(AX_USE_WEBP "Use WebP codec" ON)
option(AX_USE_JPEG "Use JPEG codec" ON)
option(AX_USE_SPINE "Use Spine" OFF)
option(AX_USE_DRAGONBONES "Use DragonBones" OFF)
option(AX_USE_LIVE2D "Use Live2D" OFF)
option(AX_USE_AUDIO "Enable audio support" ON)

# Add cmake paths
list(APPEND CMAKE_MODULE_PATH 
    "${_AX_ROOT}/cmake/Modules"
    "${_AX_ROOT}/cmake"
)

# Add our cmake modules directory to the module path
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

# Include our custom shader helpers before other includes
include(AXShaderHelpers)

# Include core build configuration
include(AXBuildHelpers)
include(AXBuildSet)

# The common cross-platforms source files and header files
file(GLOB_RECURSE GAME_HEADER
    Source/*.h
    Source/*.hpp
)
file(GLOB_RECURSE GAME_SOURCE
    Source/*.cpp
    Source/*.c
)

set(GAME_INC_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/Source"
)

# Add content folder
set(content_folder
    "${CMAKE_CURRENT_SOURCE_DIR}/Content"
)

# Set engine binary path before adding axmol subdirectory
set(ENGINE_BINARY_PATH "${CMAKE_BINARY_DIR}/engine")

# Add Axmol as subdirectory with explicit binary dir
add_subdirectory(${_AX_ROOT}/core ${ENGINE_BINARY_PATH}/axmol/core)

# Create executable
add_executable(${PROJECT_NAME} ${GAME_SOURCE} ${GAME_HEADER})

if(LINUX)
    # Add Linux-specific source files
    target_sources(${PROJECT_NAME} PRIVATE
        proj.linux/main.cpp
    )
    
    # Handle content files for Linux after target is created
    ax_mark_multi_resources(
        TARGET ${PROJECT_NAME}
        FOLDERS 
            ${CMAKE_CURRENT_SOURCE_DIR}/Resources
    )
endif()

# Include directories
target_include_directories(${PROJECT_NAME}
    PRIVATE ${GAME_INC_DIRS}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE axmol)

# Debug output for OpenAL specifically
message(STATUS "OpenAL CMake file location: ${CMAKE_CURRENT_SOURCE_DIR}/external/axmol/3rdparty/openal/CMakeLists.txt")

# Additional debug for compiler capabilities
message(STATUS "Compiler features: ${CMAKE_CXX_COMPILE_FEATURES}")

# Add cp wrapper to PATH for shader compilation
if(UNIX AND NOT APPLE)
    set(ENV{PATH} "${CMAKE_SOURCE_DIR}/tools/bin:$ENV{PATH}")
endif()

# Debug X11 search
message(STATUS "Starting X11 configuration...")

# Explicitly set X11 paths
set(X11_INCLUDE_PATH "/usr/include/X11")
set(X11_LIB_PATH "/usr/lib/x86_64-linux-gnu")

# Set explicit paths for FindX11.cmake
set(X11_X11_INCLUDE_PATH "${X11_INCLUDE_PATH}")
set(X11_X11_LIB "${X11_LIB_PATH}/libX11.so")
set(X11_Xext_INCLUDE_PATH "${X11_INCLUDE_PATH}")
set(X11_Xext_LIB "${X11_LIB_PATH}/libXext.so")

# Additional hints for FindX11
set(CMAKE_PREFIX_PATH 
    ${CMAKE_PREFIX_PATH}
    "/usr"
    "/usr/local"
)

# Debug output
message(STATUS "X11 Configuration:")
message(STATUS "  X11_INCLUDE_PATH: ${X11_INCLUDE_PATH}")
message(STATUS "  X11_LIB_PATH: ${X11_LIB_PATH}")
message(STATUS "  X11_X11_INCLUDE_PATH: ${X11_X11_INCLUDE_PATH}")
message(STATUS "  X11_X11_LIB: ${X11_X11_LIB}")

# Force pkg-config to help find X11
find_package(PkgConfig REQUIRED)
pkg_check_modules(PC_X11 REQUIRED x11)

# Now try to find X11 with our explicit configuration
find_package(X11 REQUIRED)

# Verify X11 was found
if(X11_FOUND)
    message(STATUS "X11 found successfully:")
    message(STATUS "  X11_INCLUDE_DIRS: ${X11_INCLUDE_DIRS}")
    message(STATUS "  X11_LIBRARIES: ${X11_LIBRARIES}")
else()
    message(FATAL_ERROR "X11 not found despite explicit configuration!")
endif()