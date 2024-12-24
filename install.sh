#!/bin/bash

# Get the root directory of the project (the directory where this script is located)
ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_NAME="lsd"
BUILD_DIR="$ROOT_DIR/build"
INSTALL_DIR="/usr/local/bin/"

# Ensure the install directory exists
mkdir -p "$INSTALL_DIR"

# Create or clean up the build directory
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning up old build directory..."
    rm -rf "$BUILD_DIR"
fi

echo "Creating build directory..."
mkdir "$BUILD_DIR"

# Navigate to the build directory
cd "$BUILD_DIR"

# Configure the project with cmake
echo "Running CMake..."
cmake "$ROOT_DIR" || { echo "CMake configuration failed"; exit 1; }

# Build the project
echo "Building project..."
make || { echo "Build failed"; exit 1; }

# Copy the built executable to the install directory
echo "Installing $PROJECT_NAME to $INSTALL_DIR..."
cp "$PROJECT_NAME" "$INSTALL_DIR/" || { echo "Failed to copy executable"; exit 1; }

# Navigate back to the root directory
cd "$ROOT_DIR"

# Remove the build directory
echo "Cleaning up..."
rm -rf "$BUILD_DIR"

# Confirm installation
echo "$PROJECT_NAME installed successfully in $INSTALL_DIR."
echo "Make sure $INSTALL_DIR is in your PATH."