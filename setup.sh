#!/bin/bash

# Library Management System - SFML Setup Script
# This script helps install SFML 3.0.2 and dependencies

echo "=========================================="
echo "Library Management System - Setup Script"
echo "=========================================="
echo ""

# Check if running on Linux
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "This script is designed for Linux systems."
    exit 1
fi

# Update package list
echo "Updating package list..."
sudo apt-get update

# Install build dependencies
echo ""
echo "Installing build dependencies..."
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libx11-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libudev-dev \
    libgl1-mesa-dev \
    libfreetype-dev \
    libopenal-dev \
    libvorbis-dev \
    libflac-dev

# Install fonts
echo ""
echo "Installing DejaVu fonts..."
sudo apt-get install -y fonts-dejavu fonts-dejavu-core

# Ask user if they want to build SFML from source
echo ""
echo "SFML Installation Options:"
echo "1) Build SFML 3.0.2 from source (recommended)"
echo "2) Install SFML from package manager (may be older version)"
echo "3) Skip SFML installation"
read -p "Choose an option (1-3): " choice

case $choice in
    1)
        echo ""
        echo "Building SFML 3.0.2 from source..."
        
        # Create temp directory
        mkdir -p /tmp/sfml-build
        cd /tmp/sfml-build
        
        # Clone SFML repository
        if [ -d "SFML" ]; then
            echo "SFML directory already exists, removing..."
            rm -rf SFML
        fi
        
        git clone https://github.com/SFML/SFML.git
        cd SFML
        
        # Checkout version 3.0.2 (if it exists, otherwise use master)
        echo "Checking out SFML 3.0.2..."
        git checkout 3.0.2 2>/dev/null || git checkout master
        
        # Build and install
        echo "Building SFML..."
        cmake -B build -DCMAKE_BUILD_TYPE=Release
        cmake --build build -j$(nproc)
        
        echo "Installing SFML (requires sudo)..."
        sudo cmake --install build
        
        # Update library cache
        sudo ldconfig
        
        echo "SFML installed successfully!"
        cd ~
        ;;
    2)
        echo ""
        echo "Installing SFML from package manager..."
        sudo apt-get install -y libsfml-dev
        echo "SFML installed from package manager."
        echo "Note: This may not be version 3.0.2"
        ;;
    3)
        echo "Skipping SFML installation."
        ;;
    *)
        echo "Invalid option. Exiting."
        exit 1
        ;;
esac

# Verify installation
echo ""
echo "Verifying installation..."

# Check for font
if [ -f "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf" ]; then
    echo "✓ DejaVu Sans font found"
else
    echo "✗ DejaVu Sans font not found"
    echo "  You may need to install it manually or update the font path"
fi

# Check for SFML libraries
if ldconfig -p | grep -q libsfml-graphics; then
    echo "✓ SFML graphics library found"
else
    echo "✗ SFML graphics library not found"
fi

if ldconfig -p | grep -q libsfml-window; then
    echo "✓ SFML window library found"
else
    echo "✗ SFML window library not found"
fi

if ldconfig -p | grep -q libsfml-system; then
    echo "✓ SFML system library found"
else
    echo "✗ SFML system library not found"
fi

# Check for g++
if command -v g++ &> /dev/null; then
    GCC_VERSION=$(g++ --version | head -n1)
    echo "✓ g++ found: $GCC_VERSION"
else
    echo "✗ g++ not found"
fi

echo ""
echo "=========================================="
echo "Setup complete!"
echo "=========================================="
echo ""
echo "To build the project:"
echo "  make -f Makefile_complete all"
echo ""
echo "To run the GUI version:"
echo "  ./library_system_gui"
echo ""
echo "To run the console version:"
echo "  ./library_system"
echo ""
