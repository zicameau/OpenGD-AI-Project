# check powershell
if(WIN32)
    find_program(POWERSHELL_PATH powershell)
    if(NOT POWERSHELL_PATH)
        message(STATUS "Powershell not found - some features may be limited")
    else()
        message(STATUS "Found powershell: ${POWERSHELL_PATH}")
    endif()
else()
    set(USE_POWERSHELL FALSE)
endif()

# Function to mark multiple resources
function(ax_mark_multi_resources)
    set(options)
    set(oneValueArgs TARGET)
    set(multiValueArgs FOLDERS)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ARGS_TARGET)
        message(FATAL_ERROR "ax_mark_multi_resources: TARGET argument is required")
    endif()

    if(NOT ARGS_FOLDERS)
        message(FATAL_ERROR "ax_mark_multi_resources: FOLDERS argument is required")
    endif()

    foreach(folder ${ARGS_FOLDERS})
        if(EXISTS ${folder})
            file(GLOB_RECURSE resources "${folder}/*.*")
            foreach(res ${resources})
                file(RELATIVE_PATH res_path ${CMAKE_CURRENT_SOURCE_DIR} ${res})
                get_filename_component(res_dir ${res_path} DIRECTORY)
                target_sources(${ARGS_TARGET} PRIVATE ${res})
                source_group("Resources\\${res_dir}" FILES ${res})
            endforeach()
        else()
            message(WARNING "Resource folder does not exist: ${folder}")
        endif()
    endforeach()
endfunction()

# Add compile definitions for axmol targets
function(use_ax_compile_define target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "use_ax_compile_define: ${target} is not a valid target")
    endif()
    
    target_compile_definitions(${target}
        PRIVATE
            $<$<CONFIG:Debug>:COCOS2D_DEBUG=1>
            $<$<CONFIG:Release>:NDEBUG>
            $<$<BOOL:${AX_USE_GL}>:AX_USE_GL>
            $<$<BOOL:${AX_USE_METAL}>:AX_USE_METAL>
            $<$<BOOL:${AX_USE_VULKAN}>:AX_USE_VULKAN>
            $<$<BOOL:${AX_USE_WEBGPU}>:AX_USE_WEBGPU>
            $<$<BOOL:${AX_USE_COMPAT_GL}>:AX_USE_COMPAT_GL>
            $<$<BOOL:${AX_USE_ALSOFT}>:AX_USE_ALSOFT>
    )
endfunction()

# Add compile options for axmol targets
function(use_ax_compile_options target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "use_ax_compile_options: ${target} is not a valid target")
    endif()

    if(MSVC)
        target_compile_options(${target} PRIVATE /W3 /WX-)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wno-deprecated-declarations)
    endif()
endfunction()

# Add dependencies for axmol targets
function(use_ax_depend target)
    message(STATUS "Configuring dependencies for target: ${target}")
    
    if(NOT TARGET ${target})
        message(FATAL_ERROR "use_ax_depend: ${target} is not a valid target")
    endif()

    if(UNIX AND NOT APPLE AND NOT ANDROID)
        # Debug output for link investigation
        get_target_property(existing_links ${target} LINK_LIBRARIES)
        message(STATUS "Existing link libraries for ${target}: ${existing_links}")
        
        find_package(X11 REQUIRED)
        
        message(STATUS "X11 Configuration for ${target}:")
        message(STATUS "  Include Path: ${X11_INCLUDE_DIRS}")
        message(STATUS "  Libraries: ${X11_LIBRARIES}")
        
        # Use consistent linking style
        if(existing_links)
            # If target already has plain-style links, use plain style
            target_link_libraries(${target} ${X11_LIBRARIES})
        else()
            # Otherwise use keyword style
            target_link_libraries(${target} PRIVATE ${X11_LIBRARIES})
        endif()
        
        target_include_directories(${target} PRIVATE ${X11_INCLUDE_DIRS})
    endif()
endfunction()

# Add other helper functions as needed 