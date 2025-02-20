#pragma once

#include <string>
#include <vector>
#include <functional>
#include <chrono>

struct CrashReport {
    std::string timestamp;
    std::string errorType;
    std::string stackTrace;
    std::string systemInfo;
    std::vector<std::string> logs;
};

struct TelemetryData {
    std::string sessionId;
    std::chrono::system_clock::time_point startTime;
    float uptime;
    float avgFPS;
    float peakMemoryUsage;
    std::vector<std::string> events;
};

class LinuxMaintenanceSystem {
public:
    static LinuxMaintenanceSystem* getInstance();

    // Automated Testing
    bool initializeTestPipeline();
    void scheduleTests(const std::string& testSuite);
    void runScheduledTests();
    std::vector<std::string> getTestResults();
    
    // Update Management
    bool checkForUpdates();
    bool downloadUpdate();
    bool installUpdate();
    std::string getCurrentVersion();
    
    // Crash Reporting
    void initializeCrashHandler();
    void reportCrash(const CrashReport& report);
    std::vector<CrashReport> getCrashHistory();
    
    // Telemetry
    void startTelemetrySession();
    void logTelemetryEvent(const std::string& event);
    void endTelemetrySession();
    TelemetryData getTelemetryData();

private:
    LinuxMaintenanceSystem() = default;
    std::vector<CrashReport> _crashReports;
    TelemetryData _currentSession;
    bool _telemetryEnabled = false;
}; 