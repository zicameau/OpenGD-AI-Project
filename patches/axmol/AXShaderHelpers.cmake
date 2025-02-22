# Add policy at the top of the file
if(POLICY CMP0175)
    cmake_policy(SET CMP0175 NEW)
endif()

# Function to find shader files
function(ax_find_shaders out_var)
    file(GLOB_RECURSE shader_files
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.fs"
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vs"
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.fsh"
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vsh"
    )
    set(${out_var} ${shader_files} PARENT_SCOPE)
endfunction()

# Function to handle shader compilation
function(ax_handle_shaders target)
    ax_find_shaders(shader_files)
    if(shader_files)
        target_sources(${target} PRIVATE ${shader_files})
    endif()
endfunction()

# Function to compile shaders for a target
function(ax_target_compile_shaders target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "Target ${target} does not exist")
        return()
    endif()

    # Get all shader files
    ax_find_shaders(shader_files)
    
    if(shader_files)
        # Add custom commands for each shader
        foreach(shader ${shader_files})
            get_filename_component(shader_name ${shader} NAME)
            get_filename_component(shader_ext ${shader} EXT)
            
            # Determine shader type
            if(shader_ext MATCHES "\\.(frag|fs|fsh)$")
                set(shader_type "frag")
            elseif(shader_ext MATCHES "\\.(vert|vs|vsh)$")
                set(shader_type "vert")
            else()
                message(WARNING "Unknown shader type for file: ${shader}")
                continue()
            endif()
            
            # Set output directory
            set(output_dir "${CMAKE_BINARY_DIR}/runtime/axslc")
            file(MAKE_DIRECTORY ${output_dir})
            
            # Modified custom command to remove DEPENDS when using TARGET
            add_custom_command(
                TARGET ${target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${shader} ${output_dir}/${shader_name}
                COMMENT "Copying shader ${shader_name}"
            )
        endforeach()
    endif()
endfunction() 