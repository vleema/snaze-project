cmake_minimum_required(VERSION 3.5)
project(Snaze VERSION 1.0.0 LANGUAGES CXX)

# Enable compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#=== SETTING VARIABLES === #
set(APP_NAME "snaze")
set(INCLUDE_DIR "src/include" "lib/include")
set(DEBUG_COMPILE_OPTIONS "-Wall" "-pedantic" "-g3" "-O0")
set(RELEASE_COMPILE_OPTIONS "-O3" "-DNDEBUG")

# Set sources
file(GLOB SOURCES "src/*.cpp" "lib/*.cpp")

# Include directories
include_directories(${INCLUDE_DIR})

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#=== Main App ===

# Create debug executable
add_executable(${APP_NAME}_debug ${SOURCES})
target_compile_options(${APP_NAME}_debug PRIVATE ${DEBUG_COMPILE_OPTIONS})

# Create release executable
add_executable(${APP_NAME}_release ${SOURCES})
target_compile_options(${APP_NAME}_release PRIVATE ${RELEASE_COMPILE_OPTIONS})
