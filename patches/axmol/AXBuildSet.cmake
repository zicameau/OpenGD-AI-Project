# include helper functions
include(${CMAKE_CURRENT_LIST_DIR}/AXBuildHelpers.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/AXBuildPredefinitions.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/AXShaderHelpers.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/AXOptionHelpers.cmake)
include(CMakeDependentOption)

# print cmake version
message(STATUS "CMAKE_VERSION:${CMAKE_VERSION}")

# print system info
message(STATUS "CMAKE_HOST_SYSTEM_NAME:${CMAKE_HOST_SYSTEM_NAME}")
message(STATUS "CMAKE_SYSTEM_NAME:${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_GENERATOR_PLATFORM:${CMAKE_GENERATOR_PLATFORM}")
message(STATUS "CMAKE_SYSTEM_PROCESSOR:${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CMAKE_CXX_COMPILER_ID:${CMAKE_CXX_COMPILER_ID}")

# detect compiler
if(MSVC)
    set(FUZZ_MSVC 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(FULL_CLANG 1)
endif()

message(STATUS "FUZZ_MSVC=${FUZZ_MSVC}, FULL_CLANG=${FULL_CLANG}")

# print project info
message(STATUS "PROJECT_NAME:${PROJECT_NAME}")
message(STATUS "PROJECT_SOURCE_DIR:${PROJECT_SOURCE_DIR}")
message(STATUS "_AX_ROOT:${_AX_ROOT}")
message(STATUS "CMAKE_MODULE_PATH:${CMAKE_MODULE_PATH}")
message(STATUS "PROJECT_BINARY_DIR:${PROJECT_BINARY_DIR}")
message(STATUS "ENGINE_BINARY_PATH:${ENGINE_BINARY_PATH}")

# setup architecture alias
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|AMD64)$")
    set(ARCH_ALIAS "x64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i386|i686)$")
    set(ARCH_ALIAS "x86")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
    set(ARCH_ALIAS "arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
    set(ARCH_ALIAS "arm")
endif()

message(STATUS "ARCH_ALIAS:${ARCH_ALIAS}")

# Function to handle cmake_dependent_option
macro(CMAKE_DEPENDENT_OPTION option doc default depend force)
    if("${depend}" STREQUAL "")
        set(${option} "${default}" CACHE BOOL "${doc}")
    else()
        if(${depend})
            set(${option} "${default}" CACHE BOOL "${doc}")
        else()
            set(${option} "${force}" CACHE BOOL "${doc}" FORCE)
        endif()
    endif()
endmacro() 