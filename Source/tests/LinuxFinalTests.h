#pragma once

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <functional>

class LinuxFinalTest : public ::testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;

    // UI Testing
    bool testUIComponents();
    bool testUIResponsiveness();
    bool testThemeSupport();
    
    // Notification Testing
    bool testNotificationSystem();
    bool testNotificationStyles();
    bool testNotificationInteractions();
    
    // System Tray Testing
    bool testTrayIntegration();
    bool testTrayMenu();
    bool testTrayNotifications();
    
    // Update System Testing
    bool testUpdateCheck();
    bool testUpdateDownload();
    bool testUpdateInstallation();

private:
    std::vector<std::string> _testResults;
    bool _notificationsEnabled;
    bool _trayEnabled;
}; 