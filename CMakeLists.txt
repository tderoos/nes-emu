cmake_minimum_required(VERSION 3.10)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

# Single-config generators (Makefiles, Ninja) have no default build type.
# Without one, $<CONFIG> expands empty and the NES executable collides
# with the NES build subdirectory. Default to Debug when unspecified.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Web build: opt in with -DWEB=1. The toolchain file is resolved from $EMSDK
# (exported by emsdk_env.sh), so no absolute paths are needed in IDE profiles.
# Note: $ENV{...} is NOT expanded in -DCMAKE_TOOLCHAIN_FILE on the command
# line, but it is expanded here before project() reads the variable.
if(WEB AND NOT CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{EMSDK})
	set(CMAKE_TOOLCHAIN_FILE "$ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake")
endif()

project(NES VERSION 1.0)

# target C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# supress ZERO_BUILD
# set(CMAKE_SUPPRESS_REGENERATION true)

# Emscripten: link SDL statically. CMake 4.3.0-4.3.3 has no emscripten
# shared-library support, and a static SDL is what a wasm build should
# ship anyway. (SDL3 keys off SDL_SHARED/SDL_STATIC, not BUILD_SHARED_LIBS.)
if(EMSCRIPTEN)
	set(SDL_SHARED OFF CACHE BOOL "Build a shared version of the library" FORCE)
	set(SDL_STATIC ON CACHE BOOL "Build a static version of the library" FORCE)
endif()

add_subdirectory(src)
add_subdirectory(extern/SDL EXCLUDE_FROM_ALL)
