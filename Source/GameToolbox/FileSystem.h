#pragma once

#include <string>
#include <filesystem>

namespace GameToolbox {
    class FileSystem {
    public:
        static std::string normalizePath(const std::string& path);
        static std::string getExecutablePath();
        static std::string getConfigPath();
        static std::string getUserDataPath();
        
        static bool ensureDirectoryExists(const std::string& path);
        static bool isAbsolutePath(const std::string& path);
    };
} 