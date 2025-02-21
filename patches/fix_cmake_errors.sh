#!/bin/bash

# Apply patches to fix CMake errors
patch_dir="external/axmol"

# Backup original files
cp "$patch_dir/core/CMakeLists.txt" "$patch_dir/core/CMakeLists.txt.bak"
cp "$patch_dir/3rdparty/CMakeLists.txt" "$patch_dir/3rdparty/CMakeLists.txt.bak"
cp "$patch_dir/3rdparty/yasio/CMakeLists.txt" "$patch_dir/3rdparty/yasio/CMakeLists.txt.bak"

# Apply patches
echo "Applying CMake fixes..."

# Fix cmake_dependent_option
sed -i 's/cmake_dependent_option(AX_USE_ALSOFT.*/option(AX_USE_ALSOFT "Use OpenAL-Soft audio backend" ON)/' "$patch_dir/core/CMakeLists.txt"
sed -i 's/cmake_dependent_option(AX_USE_METAL.*/option(AX_USE_METAL "Use Metal graphics API (Apple platforms only)" OFF)/' "$patch_dir/core/CMakeLists.txt"

# Fix set_target_properties
sed -i 's/set_target_properties(OpenAL PROPERTIES.*/set_target_properties(OpenAL PROPERTIES\n    RUNTIME_OUTPUT_DIRECTORY "${ENGINE_BINARY_PATH}"\n    LIBRARY_OUTPUT_DIRECTORY "${ENGINE_BINARY_PATH}"\n    ARCHIVE_OUTPUT_DIRECTORY "${ENGINE_BINARY_PATH}"\n)/' "$patch_dir/3rdparty/CMakeLists.txt"

# Add source_group_by_dir function
echo "
function(source_group_by_dir source_files)
    set(sgbd_cur_dir \${CMAKE_CURRENT_SOURCE_DIR})
    foreach(sgbd_file \${source_files})
        string(REGEX REPLACE \${sgbd_cur_dir}/\(.*\) \\\\1 sgbd_fpath \${sgbd_file})
        string(REGEX REPLACE \"\(.*\)/.*\" \\\\1 sgbd_group_name \${sgbd_fpath})
        string(COMPARE EQUAL \${sgbd_fpath} \${sgbd_group_name} sgbd_nogroup)
        string(REPLACE \"/\" \"\\\\\" sgbd_group_name \${sgbd_group_name})
        if(sgbd_nogroup)
            set(sgbd_group_name \"\\\\")
        endif(sgbd_nogroup)
        source_group(\${sgbd_group_name} FILES \${sgbd_file})
    endforeach(sgbd_file)
endfunction(source_group_by_dir)
" >> "$patch_dir/cmake/Modules/AXBuildHelpers.cmake"

echo "Patches applied successfully" 