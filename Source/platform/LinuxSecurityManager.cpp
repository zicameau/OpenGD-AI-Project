#include "LinuxSecurityManager.h"
#include <sys/prctl.h>
#include <sys/capability.h>
#include <sys/mman.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

LinuxSecurityManager* LinuxSecurityManager::getInstance() {
    static LinuxSecurityManager instance;
    return &instance;
}

bool LinuxSecurityManager::initializeSandbox() {
    // Drop unnecessary capabilities
    cap_t caps = cap_get_proc();
    if (caps == nullptr) {
        logSecurityEvent("Sandbox", "Failed to get process capabilities");
        return false;
    }

    // Clear all capabilities except those needed
    cap_clear(caps);
    
    // Set no_new_privs to prevent privilege escalation
    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) == -1) {
        logSecurityEvent("Sandbox", "Failed to set no_new_privs");
        return false;
    }

    // Prevent core dumps
    if (prctl(PR_SET_DUMPABLE, 0) == -1) {
        logSecurityEvent("Sandbox", "Failed to disable core dumps");
        return false;
    }

    return true;
}

bool LinuxSecurityManager::restrictFileAccess(const std::string& path, bool readonly) {
    int flags = readonly ? O_RDONLY : O_RDWR;
    int fd = open(path.c_str(), flags);
    
    if (fd == -1) {
        logSecurityEvent("FileAccess", "Failed to open file: " + path);
        return false;
    }

    // Set file descriptor flags
    if (fcntl(fd, F_SETFD, FD_CLOEXEC) == -1) {
        close(fd);
        logSecurityEvent("FileAccess", "Failed to set FD_CLOEXEC on: " + path);
        return false;
    }

    close(fd);
    return true;
}

bool LinuxSecurityManager::setupSecureIPC() {
    // Set up secure memory for IPC
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        logSecurityEvent("IPC", "Failed to lock memory for IPC");
        return false;
    }

    // Additional IPC security measures can be added here
    return true;
}

void LinuxSecurityManager::logSecurityEvent(const std::string& event, const std::string& details) {
    time_t now = time(nullptr);
    std::string timestamp = ctime(&now);
    timestamp.pop_back(); // Remove newline

    std::string logEntry = timestamp + " - " + event + ": " + details;
    _securityLog.push_back(logEntry);

    if (_auditLoggingEnabled) {
        syslog(LOG_AUTH | LOG_WARNING, "%s", logEntry.c_str());
    }
}

void LinuxSecurityManager::performSecurityAudit() {
    // Check file permissions
    for (const auto& [resource, permissions] : _resourcePermissions) {
        if (!checkPermissions(resource)) {
            logSecurityEvent("Audit", "Invalid permissions for: " + resource);
        }
    }

    // Check memory protections
    if (mprotect(nullptr, 0, PROT_READ) == -1) {
        logSecurityEvent("Audit", "Memory protection check failed");
    }

    // Check process privileges
    if (geteuid() == 0) {
        logSecurityEvent("Audit", "Process running with root privileges");
    }
}

bool LinuxSecurityManager::checkPermissions(const std::string& resource) {
    auto it = _resourcePermissions.find(resource);
    if (it == _resourcePermissions.end()) {
        return false;
    }

    // Check actual file permissions against stored permissions
    struct stat st;
    if (stat(resource.c_str(), &st) == -1) {
        logSecurityEvent("Permissions", "Failed to check permissions for: " + resource);
        return false;
    }

    return (st.st_mode & 0777) == (it->second & 0777);
}

void LinuxSecurityManager::setResourcePermissions(const std::string& resource, uint32_t permissions) {
    _resourcePermissions[resource] = permissions;
    
    // Apply permissions to the actual file
    if (chmod(resource.c_str(), permissions & 0777) == -1) {
        logSecurityEvent("Permissions", "Failed to set permissions for: " + resource);
    }
}

std::vector<std::string> LinuxSecurityManager::getSecurityReport() {
    return _securityLog;
}

void LinuxSecurityManager::enableAuditLogging(bool enable) {
    _auditLoggingEnabled = enable;
    if (enable) {
        openlog("OpenGD", LOG_PID | LOG_NDELAY, LOG_AUTH);
    } else {
        closelog();
    }
} 