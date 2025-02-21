# Function to parse options
function(_1kparse_option name)
    set(options "")
    set(oneValueArgs DEFAULT DESCRIPTION)
    set(multiValueArgs "")
    cmake_parse_arguments(PARSE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    if(NOT DEFINED ${name})
        set(${name} ${PARSE_DEFAULT} CACHE BOOL "${PARSE_DESCRIPTION}")
    endif()
    
    set(${name} ${${name}} PARENT_SCOPE)
endfunction()

# Function to add third party libraries
function(ax_add_3rd name)
    set(options "")
    set(oneValueArgs PATH)
    set(multiValueArgs "")
    cmake_parse_arguments(ADD "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ADD_PATH}/CMakeLists.txt")
        add_subdirectory("${ADD_PATH}")
    endif()
endfunction() 