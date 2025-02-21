function(ax_add_3rd name)
    set(options "")
    set(oneValueArgs PATH)
    set(multiValueArgs "")
    cmake_parse_arguments(ADD "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ADD_PATH}/CMakeLists.txt")
        add_subdirectory("${ADD_PATH}")
        if(TARGET ${name}_static)
            add_library(${name} ALIAS ${name}_static)
        endif()
    endif()
endfunction()

# Update chipmunk properties
if(TARGET chipmunk_static)
    set_target_properties(chipmunk_static PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        OUTPUT_NAME "chipmunk"
    )
    target_compile_definitions(chipmunk_static
        PUBLIC CP_USE_DOUBLES=0
        PUBLIC CP_USE_CGTYPES=0
    )
endif() 