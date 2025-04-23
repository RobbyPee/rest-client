#!/bin/bash

# Exit on any error
set -e

# Remove existing build directory if it exists
if [ -d "build" ]; then
    echo "Removing existing build directory..."
    rm -rf build
fi

# Create new build directory
echo "Creating new build directory..."
mkdir build
cd build

# Run CMake with sudo (for WSL)
echo "Running CMake..."
sudo cmake ..

# Build with maximum parallel jobs (9 for your system)
echo "Building..."
sudo make -j9

echo "Build complete!" 