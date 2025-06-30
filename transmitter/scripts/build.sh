#!/bin/bash

# If there is an error during the exit, stop the script.
set -e

cd ..

# Build file is created (if it exist do not create)
mkdir -p build
echo "Build file is established..."

# go to build file.
cd build

#Generate Makfile that is run Cmake
cmake ..

# Compile
make

echo "Compile is completed successfully."

