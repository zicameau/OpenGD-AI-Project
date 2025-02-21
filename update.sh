#!/bin/bash

# This script is for updating dependencies only when explicitly requested
# It should not be called automatically by setup.sh

print_status() {
    echo "===> $1"
}

update_axmol() {
    if [ -d "external/axmol" ]; then
        echo "Updating Axmol engine..."
        cd external/axmol
        git checkout dev
        git pull origin dev
        cd ../..
    else
        echo "Axmol engine directory not found!"
        exit 1
    fi
}

update_axslcc() {
    if [ -d "external/axslcc" ]; then
        echo "Updating axslcc..."
        cd external/axslcc
        git checkout main
        git pull origin main
        cd ../..
    else
        echo "axslcc directory not found!"
        exit 1
    fi
}

# Only run updates when explicitly called
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    update_axmol
    update_axslcc
fi 