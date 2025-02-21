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