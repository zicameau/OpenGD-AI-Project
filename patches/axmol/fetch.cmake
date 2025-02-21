# Lowered CMake version requirement
cmake_minimum_required(VERSION 3.22)

include(FetchContent)

# Helper function to ensure directories exist
function(ensure_directory dir)
    file(MAKE_DIRECTORY ${dir})
endfunction()

# Helper function to fetch 1k dependencies
function(_1kfetch_dist name)
    FetchContent_Declare(${name} ${ARGN})
    FetchContent_MakeAvailable(${name})
endfunction()

function(ax_fetch_content)
    FetchContent_Declare(${ARGV})
    FetchContent_MakeAvailable(${ARGV0})
endfunction() 