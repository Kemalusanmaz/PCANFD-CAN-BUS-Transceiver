#!/bin/bash

# If there is an error during the exit, stop the script.
set -e

# Find path of script (/.../PCANFD-CAN-BUS-Transceiver/bin) 
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Main project path
# SCRIPT_DIR/.. -> /.../PCANFD-CAN-BUS-Transceiver/
PROJECT_ROOT="$SCRIPT_DIR/.."

# Create executable path
EXECUTABLE_PATH="$PROJECT_ROOT/CANGuiApp/build/Desktop_Qt_6_9_1-Release/CanGuiApp"

# Check there is an executable
if [ ! -f "$EXECUTABLE_PATH" ]; then
    echo "Error: Executable can not be found!"
    echo "Correct path: $EXECUTABLE_PATH"
    exit 1
fi

echo "executable is initializing: $EXECUTABLE_PATH"
echo "----------------------------------------"

cd "$(dirname "$EXECUTABLE_PATH")"

# run executable
./"$(basename "$EXECUTABLE_PATH")" "$@"