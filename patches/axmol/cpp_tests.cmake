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

# Download test resources
_1kfetch(TEST_RES_ZIP
    URL "https://github.com/axmolengine/axmol/releases/download/v2.0.0/cpp-tests-res.zip"
    SHA256 "b9f95d8b7b3c2498a4c5f67ffb7069e1f2d70e4a6d95047a3f5c1cd4e7d1d019"
)

# Extract test resources
if(EXISTS ${TEST_RES_ZIP})
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Resources)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xzf ${TEST_RES_ZIP}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Resources
    )
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