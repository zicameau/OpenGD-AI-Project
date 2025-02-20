#include "LinuxIntegrationTests.h"
#include "LinuxPlatformManager.h"
#include "LinuxResourceManager.h"
#include "LinuxSecurityManager.h"
#include "LinuxPerformanceMonitor.h"
#include <thread>
#include <chrono>
#include <sstream>

void LinuxIntegrationTest::SetUp() {
    _isMonitoring = false;
    _integrationLog.clear();
    _performanceMetrics.clear();
}

void LinuxIntegrationTest::TearDown() {
    if (_isMonitoring) {
        stopPerformanceMonitoring();
    }
}

bool LinuxIntegrationTest::testSystemIntegration() {
    auto platform = LinuxPlatformManager::getInstance();
    auto resource = LinuxResourceManager::getInstance();
    auto security = LinuxSecurityManager::getInstance();
    auto performance = LinuxPerformanceMonitor::getInstance();

    // Test platform and resource integration
    if (!platform->handleLinux()) {
        logIntegrationEvent("Platform", "Failed to initialize");
        return false;
    }

    // Test resource and security integration
    if (!resource->loadResource("test.png")) {
        logIntegrationEvent("Resource", "Failed to load test resource");
        return false;
    }

    // Test security and performance integration
    if (!security->initializeSandbox()) {
        logIntegrationEvent("Security", "Failed to initialize sandbox");
        return false;
    }

    return true;
}

bool LinuxIntegrationTest::testComponentInteractions() {
    // Test resource loading under security constraints
    auto security = LinuxSecurityManager::getInstance();
    auto resource = LinuxResourceManager::getInstance();

    security->restrictFileAccess("./resources");
    if (resource->loadResource("./resources/test.png")) {
        logIntegrationEvent("Security", "Resource access restriction failed");
        return false;
    }

    // Test performance impact of security measures
    auto performance = LinuxPerformanceMonitor::getInstance();
    performance->beginProfile("SecurityOverhead");
    security->performSecurityAudit();
    performance->endProfile("SecurityOverhead");

    return true;
}

void LinuxIntegrationTest::startPerformanceMonitoring() {
    _isMonitoring = true;
    std::thread([this]() {
        auto performance = LinuxPerformanceMonitor::getInstance();
        while (_isMonitoring) {
            performance->beginFrame();
            _performanceMetrics.push_back(performance->getCurrentFPS());
            _performanceMetrics.push_back(performance->getCPUUsage());
            _performanceMetrics.push_back(performance->getMemoryUsage());
            performance->endFrame();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }).detach();
}

void LinuxIntegrationTest::stopPerformanceMonitoring() {
    _isMonitoring = false;
}

void LinuxIntegrationTest::runStressTest(int duration) {
    startPerformanceMonitoring();
    
    auto resource = LinuxResourceManager::getInstance();
    auto security = LinuxSecurityManager::getInstance();
    
    // Stress test resource management
    std::thread resourceStress([&]() {
        for (int i = 0; i < duration; i++) {
            resource->loadResource("test" + std::to_string(i) + ".png");
            resource->optimizeMemoryUsage();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    // Stress test security system
    std::thread securityStress([&]() {
        for (int i = 0; i < duration; i++) {
            security->performSecurityAudit();
            security->checkPermissions("test.txt");
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });

    resourceStress.join();
    securityStress.join();
    stopPerformanceMonitoring();
}

bool LinuxIntegrationTest::verifySystemStability() {
    if (_performanceMetrics.empty()) return false;

    float avgFPS = 0;
    float avgCPU = 0;
    float avgMem = 0;

    for (size_t i = 0; i < _performanceMetrics.size(); i += 3) {
        avgFPS += _performanceMetrics[i];
        avgCPU += _performanceMetrics[i + 1];
        avgMem += _performanceMetrics[i + 2];
    }

    size_t count = _performanceMetrics.size() / 3;
    avgFPS /= count;
    avgCPU /= count;
    avgMem /= count;

    return (avgFPS > 30.0f && avgCPU < 80.0f && avgMem < 90.0f);
}

void LinuxIntegrationTest::logIntegrationEvent(const std::string& component, const std::string& event) {
    std::stringstream ss;
    ss << "[" << component << "] " << event;
    _integrationLog.push_back(ss.str());
}

std::vector<std::string> LinuxIntegrationTest::getIntegrationLog() {
    return _integrationLog;
}

// Test Cases
TEST_F(LinuxIntegrationTest, SystemIntegrationTest) {
    EXPECT_TRUE(testSystemIntegration()) << "System integration test failed";
}

TEST_F(LinuxIntegrationTest, ComponentInteractionTest) {
    EXPECT_TRUE(testComponentInteractions()) << "Component interaction test failed";
}

TEST_F(LinuxIntegrationTest, StressTest) {
    runStressTest(1000);
    EXPECT_TRUE(verifySystemStability()) << "System stability test failed";
}

TEST_F(LinuxIntegrationTest, CrossComponentPerformance) {
    startPerformanceMonitoring();
    
    // Test cross-component operations
    auto resource = LinuxResourceManager::getInstance();
    auto security = LinuxSecurityManager::getInstance();
    auto performance = LinuxPerformanceMonitor::getInstance();

    for (int i = 0; i < 100; i++) {
        performance->beginProfile("CrossComponentOp");
        
        security->restrictFileAccess("test.txt");
        resource->loadResource("test.txt");
        security->checkPermissions("test.txt");
        
        performance->endProfile("CrossComponentOp");
    }

    stopPerformanceMonitoring();
    
    auto results = performance->getProfilingResults();
    EXPECT_LT(results["CrossComponentOp"], 0.1f) << "Cross-component performance below threshold";
} 