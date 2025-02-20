#include "LinuxDistributionTests.h"
#include "LinuxPlatformManager.h"
#include "LinuxResourceManager.h"
#include "LinuxSecurityManager.h"
#include "LinuxPerformanceMonitor.h"
#include <fstream>
#include <memory>
#include <thread>
#include <chrono>

void LinuxDistributionTest::SetUp() {
    _isMonitoring = false;
    _resourceData.clear();
}

void LinuxDistributionTest::TearDown() {
    if (_isMonitoring) {
        stopResourceMonitoring();
    }
}

std::string LinuxDistributionTest::getDistributionName() {
    std::ifstream osRelease("/etc/os-release");
    std::string line;
    while (std::getline(osRelease, line)) {
        if (line.find("NAME=") == 0) {
            return line.substr(5);
        }
    }
    return "Unknown";
}

std::string LinuxDistributionTest::getDistributionVersion() {
    std::ifstream osRelease("/etc/os-release");
    std::string line;
    while (std::getline(osRelease, line)) {
        if (line.find("VERSION_ID=") == 0) {
            return line.substr(11);
        }
    }
    return "Unknown";
}

bool LinuxDistributionTest::checkSystemRequirements() {
    // Check CPU
    std::ifstream cpuinfo("/proc/cpuinfo");
    std::string line;
    int cores = 0;
    while (std::getline(cpuinfo, line)) {
        if (line.find("processor") == 0) cores++;
    }
    if (cores < 2) return false;

    // Check RAM
    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        unsigned long totalRam = si.totalram * si.mem_unit;
        if (totalRam < 2UL * 1024 * 1024 * 1024) return false; // 2GB minimum
    }

    return true;
}

bool LinuxDistributionTest::checkGraphicsDrivers() {
    // Check OpenGL version
    const char* glVersion = (const char*)glGetString(GL_VERSION);
    if (!glVersion) return false;
    
    // Check for required extensions
    std::vector<std::string> requiredExtensions = {
        "GL_ARB_vertex_buffer_object",
        "GL_ARB_framebuffer_object"
    };
    
    for (const auto& ext : requiredExtensions) {
        if (!glfwExtensionSupported(ext.c_str())) {
            return false;
        }
    }
    
    return true;
}

void LinuxDistributionTest::startResourceMonitoring() {
    _isMonitoring = true;
    std::thread([this]() {
        while (_isMonitoring) {
            auto perfMon = LinuxPerformanceMonitor::getInstance();
            _resourceData.push_back(perfMon->getMemoryUsage());
            _resourceData.push_back(perfMon->getCPUUsage());
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }).detach();
}

void LinuxDistributionTest::stopResourceMonitoring() {
    _isMonitoring = false;
}

std::vector<float> LinuxDistributionTest::getResourceUsageData() {
    return _resourceData;
}

// Test Cases
TEST_F(LinuxDistributionTest, SystemCompatibility) {
    EXPECT_TRUE(checkSystemRequirements()) << "System does not meet minimum requirements";
    EXPECT_TRUE(checkGraphicsDrivers()) << "Required graphics capabilities not found";
}

TEST_F(LinuxDistributionTest, ResourceLeakCheck) {
    auto resourceMgr = LinuxResourceManager::getInstance();
    
    // Load and unload resources repeatedly
    for (int i = 0; i < 100; i++) {
        resourceMgr->loadResource("test_resource.png");
        resourceMgr->unloadResource("test_resource.png");
    }
    
    size_t finalMemory = resourceMgr->getCurrentMemoryUsage();
    EXPECT_LT(finalMemory, 1024 * 1024) << "Possible memory leak detected";
}

TEST_F(LinuxDistributionTest, SecurityVulnerabilityCheck) {
    auto securityMgr = LinuxSecurityManager::getInstance();
    
    // Test file permission controls
    EXPECT_FALSE(securityMgr->checkPermissions("/root/test.txt"));
    
    // Test sandbox restrictions
    EXPECT_TRUE(securityMgr->initializeSandbox());
    
    // Test IPC security
    EXPECT_TRUE(securityMgr->setupSecureIPC());
}

TEST_F(LinuxDistributionTest, PerformanceRegression) {
    auto perfMon = LinuxPerformanceMonitor::getInstance();
    
    startResourceMonitoring();
    
    // Run performance-intensive operations
    for (int i = 0; i < 1000; i++) {
        perfMon->beginFrame();
        // Simulate frame work
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        perfMon->endFrame();
    }
    
    stopResourceMonitoring();
    
    float avgFPS = perfMon->getCurrentFPS();
    EXPECT_GT(avgFPS, 55.0f) << "Performance below acceptable threshold";
} 