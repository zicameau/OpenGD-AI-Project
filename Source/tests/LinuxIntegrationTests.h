#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>

class LinuxIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    // System integration tests
    bool testSystemIntegration();
    bool testComponentInteractions();
    
    // Performance verification
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    std::vector<float> getPerformanceMetrics();
    
    // Stress testing
    void runStressTest(int duration);
    bool verifySystemStability();
    
    // Integration monitoring
    void logIntegrationEvent(const std::string& component, const std::string& event);
    std::vector<std::string> getIntegrationLog();

private:
    std::vector<std::string> _integrationLog;
    std::vector<float> _performanceMetrics;
    bool _isMonitoring;
}; 