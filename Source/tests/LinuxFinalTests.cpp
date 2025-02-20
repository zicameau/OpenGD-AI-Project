#include "LinuxFinalTests.h"
#include "LinuxUserInterface.h"
#include "LinuxMaintenanceSystem.h"
#include "LinuxPolishSystem.h"
#include <thread>
#include <chrono>

void LinuxFinalTest::SetUp() {
    _notificationsEnabled = true;
    _trayEnabled = true;
    _testResults.clear();
}

void LinuxFinalTest::TearDown() {
    // Cleanup any test artifacts
}

bool LinuxFinalTest::testUIComponents() {
    auto ui = LinuxUserInterface::getInstance();
    
    // Test configuration dialog
    bool configResult = ui->showConfigDialog();
    if (!configResult) {
        _testResults.push_back("Configuration dialog test failed");
        return false;
    }
    
    // Test error dialogs
    ui->showError("Test Error", "This is a test error message");
    
    // Test loading screens
    auto polish = LinuxPolishSystem::getInstance();
    LoadingScreen testScreen{
        "Testing Loading Screen",
        0.5f,
        "test_background.png",
        true
    };
    polish->showLoadingScreen(testScreen);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    polish->hideLoadingScreen();
    
    return true;
}

bool LinuxFinalTest::testUIResponsiveness() {
    auto ui = LinuxUserInterface::getInstance();
    auto polish = LinuxPolishSystem::getInstance();
    
    // Test animation speeds
    polish->setAnimationSpeed(1.0f);
    polish->enableTransitions(true);
    
    // Test rapid UI updates
    for (int i = 0; i < 10; i++) {
        polish->showDetailedError(
            "Test",
            "Rapid UI Update Test",
            "Testing UI responsiveness"
        );
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return true;
}

bool LinuxFinalTest::testNotificationSystem() {
    auto ui = LinuxUserInterface::getInstance();
    
    // Test different notification types
    ui->showUpdateNotification();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    // Test notification interaction
    bool updateResponse = ui->showUpdateDialog();
    if (!updateResponse) {
        _testResults.push_back("Update notification interaction failed");
        return false;
    }
    
    return true;
}

bool LinuxFinalTest::testTrayIntegration() {
    auto ui = LinuxUserInterface::getInstance();
    
    // Test tray initialization
    if (!ui->initializeSystemTray()) {
        _testResults.push_back("System tray initialization failed");
        return false;
    }
    
    // Test tray menu items
    ui->addTrayMenuItem("Test Item", []() {
        // Test callback
    });
    
    // Test tray icon updates
    ui->updateTrayIcon("normal.png");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ui->updateTrayIcon("active.png");
    
    return true;
}

bool LinuxFinalTest::testUpdateSystem() {
    auto maintenance = LinuxMaintenanceSystem::getInstance();
    
    // Test update check
    if (!maintenance->checkForUpdates()) {
        _testResults.push_back("Update check failed");
        return false;
    }
    
    // Test update download
    if (!maintenance->downloadUpdate()) {
        _testResults.push_back("Update download failed");
        return false;
    }
    
    // Test update progress
    auto ui = LinuxUserInterface::getInstance();
    for (float progress = 0.0f; progress <= 1.0f; progress += 0.1f) {
        ui->showUpdateProgress(progress);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return true;
}

// Test Cases
TEST_F(LinuxFinalTest, UIComponentsTest) {
    EXPECT_TRUE(testUIComponents()) << "UI components test failed";
}

TEST_F(LinuxFinalTest, NotificationSystemTest) {
    EXPECT_TRUE(testNotificationSystem()) << "Notification system test failed";
}

TEST_F(LinuxFinalTest, SystemTrayTest) {
    EXPECT_TRUE(testTrayIntegration()) << "System tray integration test failed";
}

TEST_F(LinuxFinalTest, UpdateSystemTest) {
    EXPECT_TRUE(testUpdateSystem()) << "Update system test failed";
}

TEST_F(LinuxFinalTest, UIResponsivenessTest) {
    EXPECT_TRUE(testUIResponsiveness()) << "UI responsiveness test failed";
} 