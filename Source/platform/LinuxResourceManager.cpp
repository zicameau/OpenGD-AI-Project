#include "LinuxResourceManager.h"
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <algorithm>
#include <stdexcept>
#include <syslog.h>

LinuxResourceManager* LinuxResourceManager::getInstance() {
    static LinuxResourceManager instance;
    return &instance;
}

bool LinuxResourceManager::loadResource(const std::string& path) {
    std::lock_guard<std::mutex> lock(_resourceMutex);
    try {
        // Check if resource already loaded
        if (_resources.contains(path)) {
            return true;
        }
        
        // Validate path
        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("Resource file not found: " + path);
        }
        
        // Create appropriate resource type based on extension
        auto resource = createResourceForType(path);
        if (!resource) {
            throw std::runtime_error("Unsupported resource type: " + path);
        }
        
        // Load resource and check memory limits
        if (!resource->load(path)) {
            throw std::runtime_error("Failed to load resource: " + path);
        }
        
        size_t resourceSize = resource->getMemoryUsage();
        if (_totalMemoryUsage + resourceSize > MAX_MEMORY_USAGE) {
            throw std::runtime_error("Memory limit exceeded");
        }
        
        _totalMemoryUsage += resourceSize;
        _resources[path] = std::move(resource);
        
        return true;
    } catch (const std::exception& e) {
        syslog(LOG_ERR, "Resource loading failed: %s", e.what());
        return false;
    }
}

void LinuxResourceManager::unloadResource(const std::string& path) {
    std::lock_guard<std::mutex> lock(_resourceMutex);
    try {
        auto it = _resources.find(path);
        if (it != _resources.end()) {
            _totalMemoryUsage -= it->second->getMemoryUsage();
            _resources.erase(it);
        }
    } catch (const std::exception& e) {
        syslog(LOG_ERR, "Resource unload failed: %s", e.what());
    }
}

Resource* LinuxResourceManager::getResource(const std::string& path) {
    std::lock_guard<std::mutex> lock(_resourceMutex);
    try {
        auto it = _resources.find(path);
        return (it != _resources.end()) ? it->second.get() : nullptr;
    } catch (const std::exception& e) {
        syslog(LOG_ERR, "Resource access failed: %s", e.what());
        return nullptr;
    }
}

size_t LinuxResourceManager::getCurrentMemoryUsage() const {
    std::lock_guard<std::mutex> lock(_resourceMutex);
    return _totalMemoryUsage;
}

void LinuxResourceManager::cleanup() {
    std::lock_guard<std::mutex> lock(_resourceMutex);
    try {
        _resources.clear();
        _totalMemoryUsage = 0;
    } catch (const std::exception& e) {
        syslog(LOG_ERR, "Resource cleanup failed: %s", e.what());
    }
}

LinuxResourceManager::~LinuxResourceManager() {
    cleanup();
}

void LinuxResourceManager::optimizeMemoryUsage() {
    // Get system memory info
    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        // If system memory is low, clear cache
        if (si.freeram < (si.totalram / 4)) {
            clearCache();
        }
    }

    // Remove least recently used resources
    cleanupUnusedResources();
}

void LinuxResourceManager::cleanupUnusedResources() {
    // Implement least recently used (LRU) cleanup
    std::vector<std::string> toRemove;
    
    for (const auto& [path, resource] : _resources) {
        // Add logic to track resource usage
        // Remove resources that haven't been used recently
    }

    for (const auto& path : toRemove) {
        unloadResource(path);
    }
}

void LinuxResourceManager::registerResourceCleanupHandler(
    const std::string& type, 
    std::function<void(void*)> handler) {
    _cleanupHandlers[type] = handler;
}

void LinuxResourceManager::enableResourceCaching(bool enable) {
    _cachingEnabled = enable;
    if (!enable) {
        clearCache();
    }
}

void LinuxResourceManager::setCacheSize(size_t sizeInMB) {
    _cacheSize = sizeInMB * 1024 * 1024;
    if (getCurrentMemoryUsage() > _cacheSize) {
        optimizeMemoryUsage();
    }
}

void LinuxResourceManager::clearCache() {
    // Clear cached resources but keep essential ones
    std::vector<std::string> toKeep;
    
    for (const auto& [path, resource] : _resources) {
        // Add logic to identify essential resources
        // Keep them in toKeep vector
    }

    _resources.clear();
    
    // Restore essential resources
    for (const auto& path : toKeep) {
        loadResource(path);
    }
} 