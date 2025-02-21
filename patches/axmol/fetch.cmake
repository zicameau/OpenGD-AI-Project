# Lowered CMake version requirement
cmake_minimum_required(VERSION 3.22)

include(FetchContent)

# Helper function to ensure directories exist
function(ensure_directory dir)
    file(MAKE_DIRECTORY ${dir})
endfunction()

# Function to fetch external resources
function(_1kfetch name)
    set(options "")
    set(oneValueArgs URL SHA256 FILENAME)
    set(multiValueArgs "")
    cmake_parse_arguments(FETCH "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    if(NOT FETCH_URL)
        message(FATAL_ERROR "_1kfetch: URL is required")
    endif()
    
    if(NOT FETCH_FILENAME)
        # Extract filename from URL
        string(REGEX MATCH "[^/]+$" FETCH_FILENAME "${FETCH_URL}")
    endif()
    
    # Set download path
    set(DOWNLOAD_PATH "${CMAKE_BINARY_DIR}/downloads/${FETCH_FILENAME}")
    
    # Create downloads directory if it doesn't exist
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/downloads")
    
    # Download the file if it doesn't exist or SHA256 doesn't match
    if(NOT EXISTS "${DOWNLOAD_PATH}" OR (FETCH_SHA256 AND NOT EXISTS "${DOWNLOAD_PATH}.sha256"))
        message(STATUS "Downloading ${FETCH_URL}")
        file(DOWNLOAD "${FETCH_URL}" "${DOWNLOAD_PATH}" STATUS DOWNLOAD_STATUS)
        list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
        if(NOT STATUS_CODE EQUAL 0)
            message(FATAL_ERROR "Failed to download ${FETCH_URL}")
        endif()
        
        # Verify SHA256 if provided
        if(FETCH_SHA256)
            file(SHA256 "${DOWNLOAD_PATH}" DOWNLOADED_SHA256)
            if(NOT "${DOWNLOADED_SHA256}" STREQUAL "${FETCH_SHA256}")
                message(FATAL_ERROR "SHA256 mismatch for ${FETCH_FILENAME}")
            endif()
            file(WRITE "${DOWNLOAD_PATH}.sha256" "${FETCH_SHA256}")
        endif()
    endif()
    
    # Set the output variable to the download path
    set(${name} "${DOWNLOAD_PATH}" PARENT_SCOPE)
endfunction()

function(ax_fetch_content)
    FetchContent_Declare(${ARGV})
    FetchContent_MakeAvailable(${ARGV0})
endfunction() 