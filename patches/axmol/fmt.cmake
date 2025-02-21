set(FMT_HEADERS
    include/fmt/args.h
    include/fmt/chrono.h
    include/fmt/color.h
    include/fmt/compile.h
    include/fmt/core.h
    include/fmt/format.h
    include/fmt/format-inl.h
    include/fmt/os.h
    include/fmt/ostream.h
    include/fmt/printf.h
    include/fmt/ranges.h
    include/fmt/std.h
    include/fmt/xchar.h
)

set(FMT_SOURCES
    src/format.cc
    src/os.cc
)

add_library(fmt STATIC ${FMT_SOURCES} ${FMT_HEADERS})
target_include_directories(fmt PUBLIC include)
target_compile_definitions(fmt PRIVATE FMT_EXPORT INTERFACE FMT_SHARED) 