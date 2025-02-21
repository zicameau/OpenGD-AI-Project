# Include the original shader helpers
include("${CMAKE_SOURCE_DIR}/external/axmol/cmake/Modules/AXSLCC.cmake")

# Override the shader compilation function with our wrapper
function(ax_target_compile_shaders target)
    # Create the output directory
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/runtime/axslc")
    
    # Call the original function
    _ax_target_compile_shaders(${target})
    
    if(UNIX AND NOT APPLE)
        # Modify the build commands to use cp -f instead of cp --silent
        add_custom_command(TARGET ${target} PRE_BUILD
            COMMAND sed -i "s/cp --silent/cp -f/g" 
            "${CMAKE_BINARY_DIR}/engine/axmol/core/CMakeFiles/axmol.dir/build.make"
        )
    endif()
endfunction() 