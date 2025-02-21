# Function to configure predefinitions
function(ax_config_pred)
    if(UNIX AND NOT APPLE)
        target_compile_definitions(${PROJECT_NAME} 
            PRIVATE
            CC_TARGET_OS_LINUX=1
            _GNU_SOURCE
        )
    endif()
endfunction() 