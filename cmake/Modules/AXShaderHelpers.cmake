# Include the original shader helpers
include("${CMAKE_SOURCE_DIR}/external/axmol/cmake/Modules/AXSLCC.cmake")

# Override the shader compilation function with our wrapper
function(ax_target_compile_shaders target)
    # Call the original function but redirect output to /dev/null to achieve silence
    _ax_target_compile_shaders(${target} ${ARGN})
    
    # Replace cp --silent with cp 2>/dev/null
    if(UNIX)
        execute_process(
            COMMAND sed -i "s/cp --silent/cp 2>\\/dev\\/null/g" 
            "${CMAKE_BINARY_DIR}/engine/axmol/core/CMakeFiles/axmol.dir/build.make"
        )
    endif()
endfunction() 