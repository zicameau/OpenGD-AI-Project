# Lowered CMake version requirement
cmake_minimum_required(VERSION 3.22)

include(FetchContent)

# Helper function to ensure directories exist
function(ensure_directory dir)
    file(MAKE_DIRECTORY ${dir})
endfunction()

# Helper function to fetch 1k dependencies
function(_1kfetch_dist name)
    set(options "")
    set(oneValueArgs URL URL_HASH)
    set(multiValueArgs "")
    cmake_parse_arguments(FETCH "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    FetchContent_Declare(
        ${name}
        URL ${FETCH_URL}
        URL_HASH ${FETCH_URL_HASH}
    )
    FetchContent_MakeAvailable(${name})
endfunction()

function(ax_fetch_content)
    FetchContent_Declare(${ARGV})
    FetchContent_MakeAvailable(${ARGV0})
endfunction() 