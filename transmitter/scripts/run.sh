#!/bin/bash

# If there is an error during the exit, stop the script.
set -e

# Executable path file
EXECUTABLE="../build/CanAppTransmitter"

# If it's not compile, first run build script.
if [ ! -f "$EXECUTABLE" ]; then
    echo "Program is not compiled. First ./build.sh is run..."
    ./build.sh
fi

echo "Program is working..."
$EXECUTABLE