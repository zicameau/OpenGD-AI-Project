#include "LinuxPlatformManager.h"
#include <sys/stat.h>
#include <errno.h>
#include <algorithm>
#include <syslog.h>

void LinuxPlatformManager::handleLinux() {
    std::string savePath = getSavePath();
    if (!std::filesystem::exists(savePath)) {
        if (!createDirectory(savePath)) {
            // Log error
            return;
        }
    }

    // Set up resource paths
    std::string resourcePath = getResourcePath();
    if (!std::filesystem::exists(resourcePath)) {
        // Try fallback to local resources
        resourcePath = "./resources/";
        if (!std::filesystem::exists(resourcePath)) {
            // Log error
            return;
        }
    }

    // Initialize file permissions
    mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR;  // Owner permissions
    chmod(savePath.c_str(), mode);
}

std::string LinuxPlatformManager::getStandardPath(const std::string& path) {
    std::string standardPath = path;
    std::replace(standardPath.begin(), standardPath.end(), '\\', '/');
    return standardPath;
}

std::string LinuxPlatformManager::getSaveDataPath() {
    std::string xdgData = std::getenv("XDG_DATA_HOME");
    if (xdgData.empty()) {
        xdgData = std::getenv("HOME") + std::string("/.local/share");
    }
    return xdgData + "/opengd/";
}

void LinuxPlatformManager::setSecurePermissions(const std::string& path) {
    mode_t mode = S_IRUSR | S_IWUSR;  // Read/write for owner only
    chmod(path.c_str(), mode);
}

void LinuxPlatformManager::handleError(const std::string& operation) {
    std::string error = strerror(errno);
    syslog(LOG_ERR, "OpenGD Error: %s failed: %s", operation.c_str(), error.c_str());
    // Add appropriate error recovery
}

void LinuxPlatformManager::log(LogLevel level, const std::string& message) {
    int priority;
    switch(level) {
        case LogLevel::ERROR: priority = LOG_ERR; break;
        case LogLevel::WARN: priority = LOG_WARNING; break;
        case LogLevel::INFO: priority = LOG_INFO; break;
        default: priority = LOG_DEBUG;
    }
    syslog(priority, "%s", message.c_str());
} 