# AXFetchHelpers.cmake
include(FetchContent)

function(ax_fetch_content NAME)
    cmake_parse_arguments(PARSE_ARGV 1 "ARG"
        ""
        "GIT_REPOSITORY;GIT_TAG;SOURCE_DIR;URL;URL_HASH"
        "PATCH_COMMAND"
    )

    if(ARG_GIT_REPOSITORY)
        FetchContent_Declare(${NAME}
            GIT_REPOSITORY ${ARG_GIT_REPOSITORY}
            GIT_TAG ${ARG_GIT_TAG}
            SOURCE_DIR ${ARG_SOURCE_DIR}
            PATCH_COMMAND ${ARG_PATCH_COMMAND}
        )
    elseif(ARG_URL)
        FetchContent_Declare(${NAME}
            URL ${ARG_URL}
            URL_HASH ${ARG_URL_HASH}
            SOURCE_DIR ${ARG_SOURCE_DIR}
            PATCH_COMMAND ${ARG_PATCH_COMMAND}
        )
    endif()

    FetchContent_MakeAvailable(${NAME})
endfunction() 