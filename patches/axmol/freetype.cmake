# Basic FreeType build configuration
set(VERSION_MAJOR "2")
set(VERSION_MINOR "10")
set(VERSION_PATCH "4")

# Dependencies
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)

# Source files
set(PUBLIC_HEADERS
    include/ft2build.h
    include/freetype/freetype.h
    # ... add other header files
)

set(PUBLIC_CONFIG_HEADERS
    include/freetype/config/ftconfig.h
    include/freetype/config/ftoption.h
    include/freetype/config/ftstdlib.h
)

set(SOURCE_FILES
    src/base/ftbase.c
    src/base/ftbbox.c
    # ... add other source files
)

# Build library
add_library(freetype STATIC ${SOURCE_FILES} ${PUBLIC_HEADERS} ${PUBLIC_CONFIG_HEADERS})
target_include_directories(freetype
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(freetype PRIVATE ZLIB::ZLIB PNG::PNG) 