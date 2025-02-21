cmake_policy(PUSH)
cmake_policy(SET CMP0057 NEW)

##############################################################
##  enable shader lang by shader compiler: axslcc
macro(axslcc_option variable value)
    if(NOT DEFINED ${variable})
        set(${variable} ${value} CACHE STATIC "" FORCE)
    endif()
endmacro()

axslcc_option(AXSLCC_FRAG_SOURCE_FILE_EXTENSIONS ".frag;.fsh;.fs")
axslcc_option(AXSLCC_VERT_SOURCE_FILE_EXTENSIONS ".vert;.vsh;.vs")
axslcc_option(AXSLCC_OUT_DIR ${CMAKE_BINARY_DIR}/runtime/axslc)
axslcc_option(AXSLCC_FIND_PROG_ROOT "${CMAKE_SOURCE_DIR}/external/axslcc")

find_program(AXSLCC_EXECUTABLE NAMES axslcc
    PATHS ${AXSLCC_FIND_PROG_ROOT}
    NO_DEFAULT_PATH
    REQUIRED
)

if (NOT AXSLCC_EXECUTABLE)
    message(STATUS "axslcc not found in ${AXSLCC_FIND_PROG_ROOT}")
    message(STATUS "Using custom axslcc implementation")
    set(AXSLCC_EXECUTABLE "${AXSLCC_FIND_PROG_ROOT}/axslcc")
endif()

message(STATUS "AXSLCC_EXECUTABLE=${AXSLCC_EXECUTABLE}")
message(STATUS "AXSLCC_OUT_DIR=${AXSLCC_OUT_DIR}")
message(STATUS "AXSLCC_FIND_PROG_ROOT=${AXSLCC_FIND_PROG_ROOT}")

# Include all the property definitions and functions from the original file
define_property(SOURCE PROPERTY AXSLCC_INCLUDE_DIRS
    BRIEF_DOCS "Compiled shader include directories"
    FULL_DOCS "Compiled shader include directories, seperated with comma")

define_property(SOURCE PROPERTY AXSLCC_DEFINES
    BRIEF_DOCS "Compiled shader defines"
    FULL_DOCS "Compiled shader defines, seperated with comma")

define_property(SOURCE PROPERTY AXSLCC_OUTPUT1
    BRIEF_DOCS "Compiled shader output1 additional defines"
    FULL_DOCS "Compiled shader output1 additional defines, seperated with comma")

define_property(SOURCE PROPERTY AXSLCC_OUTPUT
    BRIEF_DOCS "The compiled sources shader output path list"
    FULL_DOCS "The compiled shaders output list, seperated with comma")

function(axslcc_target_compile_shaders target)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs VERT_SOURCES FRAG_SOURCES)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(OUT_FILES)
    set(OUT_DIR "${CMAKE_BINARY_DIR}/runtime/axslc")
    file(MAKE_DIRECTORY ${OUT_DIR})

    foreach(VERT ${ARGS_VERT_SOURCES})
        get_filename_component(VERT_NAME ${VERT} NAME_WE)
        set(OUT_FILE "${OUT_DIR}/${VERT_NAME}_vs")
        
        add_custom_command(
            OUTPUT ${OUT_FILE}
            COMMAND ${CMAKE_COMMAND} -E echo "Compiling shader ${VERT} for GLSL330 ..."
            COMMAND ${AXSLCC_EXECUTABLE} -V -p 330 -o ${OUT_FILE} ${VERT}
            DEPENDS ${VERT}
            COMMENT ""
            VERBATIM
        )
        list(APPEND OUT_FILES ${OUT_FILE})
    endforeach()

    foreach(FRAG ${ARGS_FRAG_SOURCES})
        get_filename_component(FRAG_NAME ${FRAG} NAME_WE)
        set(OUT_FILE "${OUT_DIR}/${FRAG_NAME}_fs")
        
        add_custom_command(
            OUTPUT ${OUT_FILE}
            COMMAND ${CMAKE_COMMAND} -E echo "Compiling shader ${FRAG} for GLSL330 ..."
            COMMAND ${AXSLCC_EXECUTABLE} -F -p 330 -o ${OUT_FILE} ${FRAG}
            DEPENDS ${FRAG}
            COMMENT ""
            VERBATIM
        )
        list(APPEND OUT_FILES ${OUT_FILE})
    endforeach()

    add_custom_target(${target}_shaders DEPENDS ${OUT_FILES})
    add_dependencies(${target} ${target}_shaders)
endfunction()

cmake_policy(POP) 