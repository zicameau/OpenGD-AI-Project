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