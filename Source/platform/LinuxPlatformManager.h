#pragma once
#include "PlatformManager.h"
#include <filesystem>
#include <pwd.h>
#include <unistd.h>
#include <system_error>
#include <syslog.h>

class LinuxPlatformManager : public PlatformManager {
public:
    static std::string getResourcePath() override {
        const std::string defaultPath = "/usr/share/opengd/resources/";
        try {
            if (!std::filesystem::exists(defaultPath)) {
                throw std::runtime_error("Resource path does not exist: " + defaultPath);
            }
            if (access(defaultPath.c_str(), R_OK) != 0) {
                throw std::runtime_error("No read permission for resource path: " + defaultPath);
            }
            return defaultPath;
        } catch (const std::exception& e) {
            syslog(LOG_ERR, "Resource path error: %s", e.what());
            return "./resources/";  // Fallback to local path
        }
    }

    static std::string getSavePath() override {
        std::string homeDir;
        if (const char* envHome = getenv("HOME")) {
            homeDir = envHome;
        } else {
            if (struct passwd* pwd = getpwuid(getuid())) {
                homeDir = pwd->pw_dir;
            } else {
                throw std::runtime_error("Could not determine home directory");
            }
        }
        return homeDir + "/.local/share/opengd/";
    }

    static std::string pathSeparator() override {
        return "/";
    }
    
    static bool createDirectory(const std::string& path) override {
        try {
            return std::filesystem::create_directories(path);
        } catch (const std::filesystem::filesystem_error& e) {
            syslog(LOG_ERR, "Failed to create directory: %s", e.what());
            return false;
        }
    }
}; 