#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <mutex>
#include <filesystem>

class Resource {
public:
    virtual ~Resource() = default;
    virtual bool load(const std::string& path) = 0;
    virtual size_t getMemoryUsage() const = 0;
};

class LinuxResourceManager {
public:
    static LinuxResourceManager* getInstance() {
        static std::once_flag flag;
        static LinuxResourceManager* instance = nullptr;
        
        std::call_once(flag, []() {
            instance = new LinuxResourceManager();
        });
        
        return instance;
    }
    
    // Resource loading
    bool loadResource(const std::string& path);
    void unloadResource(const std::string& path);
    Resource* getResource(const std::string& path);
    
    // Memory management
    void setMemoryLimit(size_t limitInMB);
    size_t getCurrentMemoryUsage() const;
    void optimizeMemoryUsage();
    
    // Resource cleanup
    void cleanup();
    void registerResourceCleanupHandler(const std::string& type, std::function<void(void*)> handler);
    
    // Cache management
    void enableResourceCaching(bool enable);
    void setCacheSize(size_t sizeInMB);
    void clearCache();

    virtual ~LinuxResourceManager();
    
    // Delete copy and move operations
    LinuxResourceManager(const LinuxResourceManager&) = delete;
    LinuxResourceManager& operator=(const LinuxResourceManager&) = delete;
    LinuxResourceManager(LinuxResourceManager&&) = delete;
    LinuxResourceManager& operator=(LinuxResourceManager&&) = delete;

private:
    LinuxResourceManager() = default;
    std::mutex _resourceMutex;
    std::unordered_map<std::string, std::unique_ptr<Resource>> _resources;
    size_t _totalMemoryUsage = 0;
    static constexpr size_t MAX_MEMORY_USAGE = 1024 * 1024 * 1024; // 1GB
    size_t _memoryLimit = 512 * 1024 * 1024; // 512MB default
    size_t _cacheSize = 128 * 1024 * 1024;   // 128MB default
    bool _cachingEnabled = true;
    std::unordered_map<std::string, std::function<void(void*)>> _cleanupHandlers;
}; 