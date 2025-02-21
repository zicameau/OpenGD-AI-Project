# Function to configure predefinitions
function(ax_config_pred target)
    if(NOT TARGET ${target})
        add_library(${target} INTERFACE)
    endif()
    
    if(UNIX AND NOT APPLE)
        target_compile_definitions(${target} 
            INTERFACE
            CC_TARGET_OS_LINUX=1
            _GNU_SOURCE
        )
    endif()
endfunction() 