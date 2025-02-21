# Fix cmake_dependent_option usage
option(AX_USE_GL "Use OpenGL" ON)
option(AX_USE_METAL "Use Metal" OFF)
option(AX_USE_GLES "Use OpenGL ES" OFF)
set(AX_GLES_PROFILE 0 CACHE STRING "OpenGL ES Profile (0=auto, 2=2.0, 3=3.0)") 