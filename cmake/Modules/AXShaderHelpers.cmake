# Include the original shader helpers
include("${CMAKE_SOURCE_DIR}/external/axmol/cmake/Modules/AXSLCC.cmake")

# Override the shader compilation function with our wrapper
function(ax_target_compile_shaders target)
    # Ensure our wrapper is in the PATH
    if(UNIX AND NOT APPLE)
        set(ENV{PATH} "${CMAKE_SOURCE_DIR}/tools/bin:$ENV{PATH}")
    endif()
    
    # Call the original function
    _ax_target_compile_shaders(${target} ${ARGN})
endfunction() 