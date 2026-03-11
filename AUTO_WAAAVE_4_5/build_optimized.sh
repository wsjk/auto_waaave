#!/bin/bash
# Build script for optimized AUTO_WAAAVE

set -e  # Exit on error

echo "======================================"
echo "Building AUTO_WAAAVE (Optimized)"
echo "======================================"
echo ""

cd "$(dirname "$0")"

# Check if we should clean first
if [ "$1" == "clean" ]; then
    echo "Cleaning previous build..."
    make clean
    echo ""
fi

# Build with optimizations
echo "Building Release version with optimizations..."
echo "Compiler flags: -O3 -march=native -mtune=native -ffast-math"
echo ""

# Use all available cores
CORES=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
echo "Building with $CORES parallel jobs..."
echo ""

make Release -j$CORES

echo ""
echo "======================================"
echo "Build Complete!"
echo "======================================"
echo ""
echo "Binary location: bin/AUTO_WAAAVE_4_5"
echo ""
echo "To run:"
echo "  cd bin"
echo "  ./AUTO_WAAAVE_4_5"
echo ""
echo "Performance tips:"
echo "  - Monitor FPS with uncommenting FPS display in draw()"
echo "  - Run on Raspberry Pi 3 or newer for best results"
echo "  - Ensure GPU memory split is adequate (256MB recommended)"
echo ""

