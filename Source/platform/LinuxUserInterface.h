#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <mutex>
#include <once.h>

struct UIConfig {
    bool showSystemTray;
    bool enableNotifications;
    bool autoStartup;
    bool minimizeToTray;
    std::string theme;
};

class LinuxUserInterface {
public:
    static LinuxUserInterface* getInstance() {
        static std::once_flag flag;
        static LinuxUserInterface* instance = nullptr;
        
        std::call_once(flag, []() {
            instance = new LinuxUserInterface();
        });
        
        return instance;
    }
    
    virtual ~LinuxUserInterface() = default;
    
    // Delete copy and move operations
    LinuxUserInterface(const LinuxUserInterface&) = delete;
    LinuxUserInterface& operator=(const LinuxUserInterface&) = delete;
    LinuxUserInterface(LinuxUserInterface&&) = delete;
    LinuxUserInterface& operator=(LinuxUserInterface&&) = delete;

    // Error Handling
    void showError(const std::string& title, const std::string& message);
    void setErrorCallback(std::function<void(const std::string&)> callback);
    
    // Configuration UI
    bool showConfigDialog();
    UIConfig getConfig();
    void setConfig(const UIConfig& config);
    
    // System Tray
    bool initializeSystemTray();
    void updateTrayIcon(const std::string& iconPath);
    void addTrayMenuItem(const std::string& label, std::function<void()> callback);
    
    // Auto-Update UI
    void showUpdateNotification();
    bool showUpdateDialog();
    void showUpdateProgress(float progress);

private:
    LinuxUserInterface() = default;
    std::mutex _configMutex;
    std::mutex _trayMutex;
    UIConfig _config;
    std::vector<std::pair<std::string, std::function<void()>>> _trayMenuItems;
    std::function<void(const std::string&)> _errorCallback;
}; 