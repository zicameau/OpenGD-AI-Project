# Include the original shader helpers
include("${CMAKE_SOURCE_DIR}/external/axmol/cmake/Modules/AXSLCC.cmake")

# Override the shader compilation function with our wrapper
function(ax_target_compile_shaders target)
    # Create the output directory first
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/runtime/axslc")
    
    # Call the original function
    _ax_target_compile_shaders(${target})
    
    if(UNIX AND NOT APPLE)
        # Fix the cp commands in the build file
        add_custom_command(
            TARGET ${target} PRE_BUILD
            COMMAND sed -i "s/cp --silent/cp -f/g" 
            "${CMAKE_BINARY_DIR}/engine/axmol/core/CMakeFiles/axmol.dir/build.make"
        )
    endif()
endfunction() 