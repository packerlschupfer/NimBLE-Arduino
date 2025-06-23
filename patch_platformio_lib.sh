#!/bin/bash

# Script to patch PlatformIO's installed NimBLE-Arduino library with observer-only fixes

echo "Patching PlatformIO NimBLE-Arduino library with observer-only fixes..."

# Find the PlatformIO library directory
PIO_LIB_DIR=""

# Check common locations
if [ -d ".pio/libdeps" ]; then
    # Project-specific library
    PIO_LIB_DIR=$(find .pio/libdeps -name "NimBLE-Arduino" -type d | head -1)
elif [ -d "$HOME/.platformio/packages/framework-arduinoespressif32/libraries/NimBLE-Arduino" ]; then
    # Global ESP32 Arduino framework
    PIO_LIB_DIR="$HOME/.platformio/packages/framework-arduinoespressif32/libraries/NimBLE-Arduino"
fi

if [ -z "$PIO_LIB_DIR" ] || [ ! -d "$PIO_LIB_DIR" ]; then
    echo "ERROR: Could not find NimBLE-Arduino library directory"
    echo "Please ensure NimBLE-Arduino is installed via PlatformIO"
    exit 1
fi

echo "Found NimBLE-Arduino at: $PIO_LIB_DIR"

# Get the directory of this script (where our fixed files are)
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Files to copy
FILES_TO_COPY=(
    "src/nimconfig_observer_only.h"
    "src/NimBLEObserverOnly.h"
    "src/NimBLEObserverOnly.cpp"
    "src/NimBLEScan.cpp"
    "src/NimBLEDevice.cpp"
    "src/nimble_stubs.cpp"
)

# Copy each file
for file in "${FILES_TO_COPY[@]}"; do
    if [ -f "$SCRIPT_DIR/$file" ]; then
        echo "Copying $file..."
        cp "$SCRIPT_DIR/$file" "$PIO_LIB_DIR/$file"
        if [ $? -eq 0 ]; then
            echo "  ✓ Success"
        else
            echo "  ✗ Failed to copy $file"
        fi
    else
        echo "  ⚠ Warning: $file not found in source"
    fi
done

# Copy documentation files
echo ""
echo "Copying documentation..."
for doc in OBSERVER_ONLY_*.md; do
    if [ -f "$SCRIPT_DIR/$doc" ]; then
        cp "$SCRIPT_DIR/$doc" "$PIO_LIB_DIR/"
        echo "  ✓ Copied $doc"
    fi
done

# Copy example if it doesn't exist
if [ ! -d "$PIO_LIB_DIR/examples/Observer_Only_Test" ]; then
    echo ""
    echo "Copying Observer_Only_Test example..."
    mkdir -p "$PIO_LIB_DIR/examples/Observer_Only_Test"
    cp -r "$SCRIPT_DIR/examples/Observer_Only_Test/"* "$PIO_LIB_DIR/examples/Observer_Only_Test/"
    echo "  ✓ Example copied"
fi

echo ""
echo "Patching complete! The warnings should now be fixed."
echo ""
echo "To use observer-only mode in your project:"
echo "1. Include the header at the very beginning of your main.cpp:"
echo "   #include \"nimconfig_observer_only.h\""
echo "2. Use NimBLEObserverOnly instead of NimBLEDevice"
echo ""
echo "Note: You may need to clean and rebuild your project:"
echo "  pio run --target clean"
echo "  pio run"