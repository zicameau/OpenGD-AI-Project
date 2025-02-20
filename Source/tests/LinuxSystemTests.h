#pragma once
#include <gtest/gtest.h>
#include "../platform/LinuxWindowManager.h"
#include "../platform/LinuxInputManager.h"
#include "../platform/LinuxAudioManager.h"

class LinuxSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        windowManager = LinuxWindowManager::getInstance();
        inputManager = LinuxInputManager::getInstance();
        audioManager = LinuxAudioManager::getInstance();
    }

    void TearDown() override {
        if (windowManager) windowManager->cleanup();
        if (inputManager) inputManager->cleanup();
        if (audioManager) audioManager->cleanup();
    }

    LinuxWindowManager* windowManager;
    LinuxInputManager* inputManager;
    LinuxAudioManager* audioManager;
}; 