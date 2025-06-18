# mkdir -p build  => create build file
# cd build => enter the build file
# cmake .. => create makefile
# make => build makefile

# Minimum Cmake version
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(CanAppTransmitter VERSION 1.0)

# build with debug mode
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g") # add -g (debug symbol) in the c++ compiler)

# the location of Boost library that Cmake will find.
find_package(Boost REQUIRED COMPONENTS system)

# the path of header file
include_directories(include ../configuration/include)

link_directories(${CMAKE_SOURCE_DIR}/driver/lib)

# definition of source file
set(SOURCES
    ../configuration/src/configuration.cpp src/transmit.cpp src/main.cpp )

# define compile target
add_executable(CanAppTransmitter ${SOURCES})
target_link_libraries(CanAppTransmitter pcanfd)  # libpcan.so'yu bağlar
