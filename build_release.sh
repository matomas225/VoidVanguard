#!/bin/bash

# Build release script for VoidVanguard

echo "Building Windows release..."

# Clean and build Windows version
make clean
make windows

# Create or update Windows release directory
mkdir -p VoidVanguard-Windows

# Copy executable and required files
cp VoidVanguard.exe VoidVanguard-Windows/
cp /home/matomas/SDL2-windows/x86_64-w64-mingw32/bin/SDL2.dll /home/matomas/SDL2-windows/x86_64-w64-mingw32/bin/SDL2_mixer.dll VoidVanguard-Windows/
cp *.wav *.mp3 VoidVanguard-Windows/

echo "Windows release ready in VoidVanguard-Windows/"

echo "Building Linux release..."

# Clean and build Linux version
make clean
make

# Create Linux release directory
mkdir -p VoidVanguard-Linux

# Copy executable and assets
cp VoidVanguard VoidVanguard-Linux/
cp *.wav *.mp3 VoidVanguard-Linux/

echo "Linux release ready in VoidVanguard-Linux/"