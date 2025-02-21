cmake_minimum_required(VERSION 3.10)

set(APP_NAME cpp-tests)

# Add source files
file(GLOB_RECURSE APP_SOURCES 
    Classes/*.cpp
    Classes/*.c
)

file(GLOB_RECURSE APP_HEADERS 
    Classes/*.h
    Classes/*.hpp
)

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/Resources)
    # Try to download the resources
    set(RES_URL "https://github.com/axmolengine/axmol/releases/download/v2.0.0/cpp-tests-res.zip")
    set(RES_ZIP "${CMAKE_CURRENT_BINARY_DIR}/cpp-tests-res.zip")
    
    message(STATUS "Downloading cpp-tests resources...")
    file(DOWNLOAD ${RES_URL} ${RES_ZIP} STATUS DOWNLOAD_STATUS)
    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    
    if(STATUS_CODE EQUAL 0)
        # Extract the zip file
        message(STATUS "Extracting resources...")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xzf ${RES_ZIP}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
    else()
        message(STATUS "Failed to download resources, copying from source...")
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    endif()
endif()

# Create the executable
add_executable(${APP_NAME} ${APP_SOURCES} ${APP_HEADERS})

# Link libraries
target_link_libraries(${APP_NAME} PRIVATE axmol)

# Include directories
target_include_directories(${APP_NAME} PRIVATE Classes)

# Set properties
set_target_properties(${APP_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Copy resources
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Resources)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif() 