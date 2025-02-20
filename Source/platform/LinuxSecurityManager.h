#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

class LinuxSecurityManager {
public:
    static LinuxSecurityManager* getInstance();

    // Sandboxing
    bool initializeSandbox();
    bool restrictFileAccess(const std::string& path, bool readonly = true);
    bool restrictSystemAccess();
    
    // IPC Security
    bool setupSecureIPC();
    bool verifyIPCConnection(int connectionId);
    void closeIPCConnection(int connectionId);
    
    // Security Logging
    void logSecurityEvent(const std::string& event, const std::string& details);
    void enableAuditLogging(bool enable);
    
    // Security Audit
    void performSecurityAudit();
    std::vector<std::string> getSecurityReport();
    
    // Access Control
    bool checkPermissions(const std::string& resource);
    void setResourcePermissions(const std::string& resource, uint32_t permissions);

private:
    LinuxSecurityManager() = default;
    
    std::unordered_map<std::string, uint32_t> _resourcePermissions;
    std::vector<std::string> _securityLog;
    bool _auditLoggingEnabled = false;
}; 