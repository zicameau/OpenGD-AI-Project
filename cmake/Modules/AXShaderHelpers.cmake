# Include the original shader helpers
include("${CMAKE_SOURCE_DIR}/external/axmol/cmake/Modules/AXSLCC.cmake")

# Override the shader compilation function with our wrapper
function(ax_target_compile_shaders target)
    # Call the original function but redirect output to /dev/null to achieve silence
    _ax_target_compile_shaders(${target} ${ARGN})
    
    if(UNIX)
        # Add a custom command to modify the build.make file after CMake generation
        add_custom_command(
            TARGET ${target} PRE_BUILD
            COMMAND sed -i "s/cp --silent/cp -f/g" 
            "${CMAKE_BINARY_DIR}/engine/axmol/core/CMakeFiles/axmol.dir/build.make"
        )
    endif()
endfunction() 