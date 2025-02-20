#pragma once

#include <string>

class PlatformManager {
public:
    static std::string getResourcePath();
    static std::string getSavePath();
    static std::string pathSeparator();
    
    static bool createDirectory(const std::string& path);
    static bool fileExists(const std::string& path);
}; 