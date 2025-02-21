# Include the original shader helpers
include("${CMAKE_SOURCE_DIR}/external/axmol/cmake/Modules/AXSLCC.cmake")

# Override the shader compilation function with our wrapper
function(ax_target_compile_shaders target)
    # Create the output directory
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/runtime/axslc")
    
    # Call the original function
    _ax_target_compile_shaders(${target})
    
    if(UNIX AND NOT APPLE)
        # Add a custom command to modify all build.make files
        add_custom_command(TARGET ${target} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Patching build files for shader compilation..."
            COMMAND find "${CMAKE_BINARY_DIR}" -name "build.make" -type f -exec sed -i "s/cp --silent/cp -f/g" {} +
            VERBATIM
        )
    endif()
endfunction() 