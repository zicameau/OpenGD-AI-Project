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

# Add other helper functions as needed 