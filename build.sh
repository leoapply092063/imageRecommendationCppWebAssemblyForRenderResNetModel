#!/bin/bash

# Build script for the image recommendation system

echo "Building image recommendation system..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake ..

# Build the project
echo "Building the project..."
make -j$(nproc)

# Copy executable to project root
cp feature_extractor ../

echo "Build completed successfully!"
echo "You can now run: npm start" 