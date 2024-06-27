cmake_minimum_required(VERSION 3.5)
project(Snaze VERSION 1.0.0 LANGUAGES CXX)

# Enable compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#=== SETTING VARIABLES === #
set(APP_NAME "snaze")
set(INCLUDE_DIR "src/include" "lib/include")
set(COMPILE_OPTIONS ${CMAKE_CXX_FLAGS} "-Wall" "-pedantic" "-g3")

# Set sources
file(GLOB SOURCES "src/*.cpp" "lib/*.cpp")

# Include directories
include_directories(${INCLUDE_DIR})

# Set compiler flags
add_compile_options(${COMPILE_OPTIONS})

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#=== Main App ===

# Create executable
add_executable(${APP_NAME} ${SOURCES})
