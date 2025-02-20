#!/bin/bash

# Script to run OpenGD Linux system tests

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo "Running OpenGD Linux System Tests..."

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Build tests
cmake .. -DBUILD_TESTING=ON
make run_tests

# Run tests
if ./OpenGDTests; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed!${NC}"
    exit 1
fi 