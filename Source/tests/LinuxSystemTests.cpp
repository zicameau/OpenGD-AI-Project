#include "LinuxSystemTests.h"
#include <thread>
#include <chrono>

// Window Management Tests
TEST_F(LinuxSystemTest, WindowCreation) {
    ASSERT_TRUE(windowManager->init(800, 600, "Test Window"));
    EXPECT_FALSE(windowManager->shouldClose());
    
    int width, height;
    windowManager->getSize(width, height);
    EXPECT_EQ(width, 800);
    EXPECT_EQ(height, 600);
}

// Input System Tests
TEST_F(LinuxSystemTest, InputInitialization) {
    ASSERT_TRUE(windowManager->init(800, 600, "Test Window"));
    ASSERT_TRUE(inputManager->init());
    
    // Test key mapping
    EXPECT_FALSE(inputManager->isKeyPressed(XK_space));
    EXPECT_TRUE(inputManager->isActionPressed("jump") == false);
}

// Audio System Tests
TEST_F(LinuxSystemTest, AudioPlayback) {
    ASSERT_TRUE(audioManager->init());
    
    // Test audio playback
    int soundId = audioManager->playSound("test.wav");
    EXPECT_GT(soundId, 0);
    EXPECT_TRUE(audioManager->isPlaying(soundId));
    
    // Test volume control
    audioManager->setVolume(soundId, 0.5f);
    audioManager->stopSound(soundId);
    EXPECT_FALSE(audioManager->isPlaying(soundId));
}

// Performance Tests
TEST_F(LinuxSystemTest, InputLatency) {
    ASSERT_TRUE(windowManager->init(800, 600, "Test Window"));
    ASSERT_TRUE(inputManager->init());
    
    auto start = std::chrono::high_resolution_clock::now();
    for(int i = 0; i < 1000; i++) {
        inputManager->update();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // Average latency should be less than 1ms per update
    EXPECT_LT(duration.count() / 1000.0, 1000);
}

// Integration Tests
TEST_F(LinuxSystemTest, SystemIntegration) {
    ASSERT_TRUE(windowManager->init(800, 600, "Test Window"));
    ASSERT_TRUE(inputManager->init());
    ASSERT_TRUE(audioManager->init());
    
    // Test complete game loop cycle
    windowManager->processEvents();
    inputManager->update();
    
    int soundId = audioManager->playSound("test.wav");
    EXPECT_GT(soundId, 0);
    
    // Simulate game loop for 100ms
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    windowManager->swapBuffers();
    EXPECT_FALSE(windowManager->shouldClose());
} 