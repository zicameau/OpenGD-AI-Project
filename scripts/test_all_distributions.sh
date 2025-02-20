#!/bin/bash

# Master script to run tests across all supported distributions using Docker

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

# Test results array
declare -A results

function run_distro_test() {
    local distro=$1
    local tag=$2
    
    echo "Testing on $distro..."
    
    # Build Docker image
    docker build -t opengd-test-$distro -f docker/$distro.Dockerfile .
    
    # Run tests in container
    if docker run --rm \
        -v $(pwd):/opengd \
        opengd-test-$distro \
        /opengd/scripts/dist_tests/${distro}_test.sh; then
        results[$distro]="PASSED"
    else
        results[$distro]="FAILED"
    fi
}

# Run tests for each distribution
run_distro_test "ubuntu" "latest"
run_distro_test "fedora" "latest"
run_distro_test "arch" "latest"

# Print results
echo -e "\nTest Results:"
echo "=============="
for distro in "${!results[@]}"; do
    if [ "${results[$distro]}" == "PASSED" ]; then
        echo -e "$distro: ${GREEN}${results[$distro]}${NC}"
    else
        echo -e "$distro: ${RED}${results[$distro]}${NC}"
    fi
done 