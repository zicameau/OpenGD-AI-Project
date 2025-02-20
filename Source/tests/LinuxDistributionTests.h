#pragma once

#include <gtest/gtest.h>
#include <string>
#include <vector>

class LinuxDistributionTest : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    // Distribution detection
    std::string getDistributionName();
    std::string getDistributionVersion();
    
    // System requirements
    bool checkSystemRequirements();
    bool checkGraphicsDrivers();
    bool checkAudioSystem();
    
    // Resource monitoring
    void startResourceMonitoring();
    void stopResourceMonitoring();
    std::vector<float> getResourceUsageData();

private:
    std::vector<float> _resourceData;
    bool _isMonitoring;
}; 