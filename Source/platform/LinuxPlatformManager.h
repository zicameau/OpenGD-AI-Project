#pragma once
#include "PlatformManager.h"
#include <filesystem>
#include <pwd.h>
#include <unistd.h>

class LinuxPlatformManager : public PlatformManager {
public:
    static std::string getResourcePath() override {
        return "/usr/share/opengd/resources/";
    }

    static std::string getSavePath() override {
        const char* homeDir = getenv("HOME");
        if (!homeDir) {
            struct passwd* pwd = getpwuid(getuid());
            if (pwd)
                homeDir = pwd->pw_dir;
        }
        return std::string(homeDir) + "/.local/share/opengd/";
    }

    static std::string pathSeparator() override {
        return "/";
    }
    
    static bool createDirectory(const std::string& path) override {
        return std::filesystem::create_directories(path);
    }
}; 