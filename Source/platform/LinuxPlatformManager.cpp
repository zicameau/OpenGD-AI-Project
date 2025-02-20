#include "LinuxPlatformManager.h"
#include <sys/stat.h>
#include <errno.h>

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