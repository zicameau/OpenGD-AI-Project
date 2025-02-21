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

# Explicitly set X11 paths
set(X11_INCLUDE_DIR "/usr/include/X11")
set(X11_LIBRARIES "/usr/lib/x86_64-linux-gnu")
set(X11_X11_LIB "/usr/lib/x86_64-linux-gnu/libX11.so")
set(X11_X11_INCLUDE_PATH "/usr/include/X11")

# Add X11 to the module path
list(APPEND CMAKE_MODULE_PATH "/usr/lib/x86_64-linux-gnu/cmake")

# Force pkg-config to look in the right place
set(ENV{PKG_CONFIG_PATH} "/usr/lib/x86_64-linux-gnu/pkgconfig:$ENV{PKG_CONFIG_PATH}")

# Enable language support
enable_language(CXX)

if(POLICY CMP0175)
    cmake_policy(SET CMP0175 NEW)
endif()

cmake_minimum_required(VERSION 3.22)

project(OpenGD_Game)

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