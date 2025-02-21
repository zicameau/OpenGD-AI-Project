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

cmake_minimum_required(VERSION 3.22)

project(OpenGD_Game)

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

# Include core build configuration
include(AXBuildHelpers)
include(AXBuildSet)

# Add Axmol as subdirectory
add_subdirectory(${_AX_ROOT})

# Add your project's source files
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE axmol)