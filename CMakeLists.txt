cmake_minimum_required(VERSION 3.10)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

# Single-config generators (Makefiles, Ninja) have no default build type.
# Without one, $<CONFIG> expands empty and the NES executable collides
# with the NES build subdirectory. Default to Debug when unspecified.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

project(NES VERSION 1.0)

# target C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# supress ZERO_BUILD
# set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory(src)
add_subdirectory(extern/SDL EXCLUDE_FROM_ALL)
