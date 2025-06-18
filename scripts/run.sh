#!/bin/bash

# If there is an error during the exit, stop the script.
set -e
cd ..
# go to build file.
cd build

# Check there is an executable which has named Qemu1553Client. 
if [ -f "CanAppTransmitter" ]; then
#   echo "Program is working..."
  ./CanAppReceiver
  ./CanAppTransmitter
else
  echo "Error: The binary file cannot be found. First. run ./build.sh"
  exit 1
fi