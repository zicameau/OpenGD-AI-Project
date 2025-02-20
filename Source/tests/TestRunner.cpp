#include <gtest/gtest.h>
#include <iostream>
#include "LinuxSystemTests.h"

int main(int argc, char** argv) {
    std::cout << "Running OpenGD Linux System Tests\n" << std::endl;
    
    testing::InitGoogleTest(&argc, argv);
    
    // Add test event listeners
    testing::TestEventListeners& listeners = testing::UnitTest::GetInstance()->listeners();
    listeners.Append(new testing::PrintToStdoutAndStderr);
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    std::cout << "\nTest execution completed." << std::endl;
    return result;
} 