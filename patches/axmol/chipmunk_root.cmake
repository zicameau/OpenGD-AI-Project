cmake_minimum_required(VERSION 3.0)

project(Chipmunk2D VERSION 7.0.3)

# Options
option(BUILD_DEMOS "Build the demo application" OFF)
option(BUILD_SHARED "Build and install the shared library" OFF)
option(BUILD_STATIC "Build and install the static library" ON)
option(INSTALL_STATIC "Install the static library" ON)
option(BUILD_DOCS "Build the documentation" OFF)

add_subdirectory(src) 