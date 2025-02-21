# Lowered CMake version requirement
cmake_minimum_required(VERSION 3.22)

include(FetchContent)

function(ax_fetch_content)
    FetchContent_Declare(${ARGV})
    FetchContent_MakeAvailable(${ARGV0})
endfunction() 