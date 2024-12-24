# Specify the minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(lsd)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the source directory
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Find all source and header files in the /src directory
file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${SRC_DIR}/*.h)

# Set the output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Add an executable target
add_executable(lsd ${SOURCES} ${HEADERS})

# Link include directories specifically to the target
target_include_directories(lsd PRIVATE ${SRC_DIR})

# Set flags for debug and release configurations
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")