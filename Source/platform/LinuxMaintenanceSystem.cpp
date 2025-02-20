#include "LinuxMaintenanceSystem.h"
#include "LinuxPerformanceMonitor.h"
#include <execinfo.h>
#include <signal.h>
#include <unistd.h>
#include <curl/curl.h>
#include <sstream>
#include <ctime>
#include <filesystem>

LinuxMaintenanceSystem* LinuxMaintenanceSystem::getInstance() {
    static LinuxMaintenanceSystem instance;
    return &instance;
}

bool LinuxMaintenanceSystem::initializeTestPipeline() {
    // Set up automated test environment
    std::filesystem::create_directories("./test_results");
    
    // Initialize test framework
    testing::InitGoogleTest();
    
    // Set up test listeners
    testing::TestEventListeners& listeners = testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new CustomTestListener());
    
    return true;
}

void LinuxMaintenanceSystem::scheduleTests(const std::string& testSuite) {
    // Add test to scheduled queue
    std::ofstream schedule("./test_results/schedule.txt", std::ios::app);
    schedule << testSuite << std::endl;
}

void LinuxMaintenanceSystem::runScheduledTests() {
    // Read scheduled tests
    std::ifstream schedule("./test_results/schedule.txt");
    std::string testSuite;
    
    while (std::getline(schedule, testSuite)) {
        // Run each test suite
        std::string command = "./run_tests --gtest_filter=" + testSuite;
        system(command.c_str());
    }
    
    // Clear schedule after running
    std::ofstream clear("./test_results/schedule.txt", std::ios::trunc);
}

std::vector<std::string> LinuxMaintenanceSystem::getTestResults() {
    std::vector<std::string> results;
    for (const auto& entry : std::filesystem::directory_iterator("./test_results")) {
        if (entry.path().extension() == ".xml") {
            std::ifstream result(entry.path());
            std::string line;
            while (std::getline(result, line)) {
                results.push_back(line);
            }
        }
    }
    return results;
}

bool LinuxMaintenanceSystem::checkForUpdates() {
    CURL* curl = curl_easy_init();
    if (!curl) return false;
    
    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.opengd.org/version");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    
    return (res == CURLE_OK && response > getCurrentVersion());
}

bool LinuxMaintenanceSystem::downloadUpdate() {
    CURL* curl = curl_easy_init();
    if (!curl) return false;
    
    FILE* fp = fopen("update.tar.gz", "wb");
    if (!fp) return false;
    
    curl_easy_setopt(curl, CURLOPT_URL, "https://api.opengd.org/latest");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    fclose(fp);
    
    return (res == CURLE_OK);
}

void LinuxMaintenanceSystem::initializeCrashHandler() {
    signal(SIGSEGV, [](int sig) {
        void* array[50];
        size_t size = backtrace(array, 50);
        
        CrashReport report;
        report.timestamp = std::to_string(std::time(nullptr));
        report.errorType = "SIGSEGV";
        report.systemInfo = "Linux " + std::string(sysinfo.release);
        
        char** messages = backtrace_symbols(array, size);
        for (size_t i = 0; i < size; ++i) {
            report.stackTrace += messages[i] + std::string("\n");
        }
        free(messages);
        
        getInstance()->reportCrash(report);
        exit(1);
    });
}

void LinuxMaintenanceSystem::reportCrash(const CrashReport& report) {
    _crashReports.push_back(report);
    
    // Save crash report to disk
    std::ofstream crashLog("./crashes/" + report.timestamp + ".log");
    crashLog << "Error Type: " << report.errorType << std::endl;
    crashLog << "Stack Trace:\n" << report.stackTrace << std::endl;
    crashLog << "System Info: " << report.systemInfo << std::endl;
    
    // Upload crash report if enabled
    if (_telemetryEnabled) {
        // Upload logic here
    }
}

void LinuxMaintenanceSystem::startTelemetrySession() {
    _telemetryEnabled = true;
    _currentSession.sessionId = std::to_string(std::time(nullptr));
    _currentSession.startTime = std::chrono::system_clock::now();
}

void LinuxMaintenanceSystem::logTelemetryEvent(const std::string& event) {
    if (!_telemetryEnabled) return;
    
    _currentSession.events.push_back(event);
    
    auto perfMon = LinuxPerformanceMonitor::getInstance();
    _currentSession.avgFPS = perfMon->getCurrentFPS();
    _currentSession.peakMemoryUsage = perfMon->getMemoryUsage();
}

void LinuxMaintenanceSystem::endTelemetrySession() {
    if (!_telemetryEnabled) return;
    
    auto endTime = std::chrono::system_clock::now();
    _currentSession.uptime = std::chrono::duration<float>(
        endTime - _currentSession.startTime).count();
    
    // Save telemetry data
    std::ofstream telemetryLog("./telemetry/" + _currentSession.sessionId + ".log");
    telemetryLog << "Session Duration: " << _currentSession.uptime << "s\n";
    telemetryLog << "Average FPS: " << _currentSession.avgFPS << "\n";
    telemetryLog << "Peak Memory Usage: " << _currentSession.peakMemoryUsage << "MB\n";
    
    for (const auto& event : _currentSession.events) {
        telemetryLog << "Event: " << event << "\n";
    }
}

TelemetryData LinuxMaintenanceSystem::getTelemetryData() {
    return _currentSession;
} 