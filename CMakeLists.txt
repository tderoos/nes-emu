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

# Warnings for our own targets only (not SDL)
add_compile_options($<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wall$<COMMA>-Wextra>)

# LTO in release builds (flattens the CPU/PPU hot paths across the
# core library boundary)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

# RelWithDebInfo: optimized + debuggable - useful for emulator profiling
# (CMake provides the flags; add the profile in CLion: Settings > Build,
# Execution, Deployment > CMake > + > Build type: RelWithDebInfo)

# supress ZERO_BUILD
# set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory(src/cores/nes)
add_subdirectory(extern/SDL EXCLUDE_FROM_ALL)

add_library(Common
			src/common/Thread.cpp)
target_include_directories(Common PUBLIC src)
target_link_libraries(Common PUBLIC SDL3::SDL3)

add_executable(NES src/frontend/main.cpp)
target_include_directories(NES PUBLIC src)
target_link_libraries(NES PUBLIC LibCoreNES)
target_link_libraries(NES PUBLIC Common)
target_link_libraries(NES PUBLIC SDL3::SDL3)
target_compile_definitions(NES PUBLIC SDL_MAIN_USE_CALLBACKS)
