# Include the original shader helpers
include("${CMAKE_SOURCE_DIR}/external/axmol/cmake/Modules/AXSLCC.cmake")

# Override the shader compilation function
function(ax_target_compile_shaders target)
    # Create the output directory
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/runtime/axslc")
    
    # Get all shader files
    ax_find_shaders(shader_files)
    
    if(shader_files)
        foreach(shader ${shader_files})
            get_filename_component(shader_name ${shader} NAME)
            get_filename_component(shader_ext ${shader} EXT)
            
            # Determine shader type and output name
            if(shader_ext MATCHES "\\.(frag|fs|fsh)$")
                set(output_name "${shader_name}_fs")
            elseif(shader_ext MATCHES "\\.(vert|vs|vsh)$")
                set(output_name "${shader_name}_vs")
            else()
                message(WARNING "Unknown shader type for file: ${shader}")
                continue()
            endif()
            
            # Use CMake's file(COPY) instead of cp
            add_custom_command(
                TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${shader}"
                    "${CMAKE_BINARY_DIR}/runtime/axslc/${output_name}"
                DEPENDS "${shader}"
                COMMENT "Copying shader ${shader_name}"
                VERBATIM
            )
        endforeach()
    endif()
endfunction() 