cmake_minimum_required(VERSION 3.10)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>")

project(NES VERSION 1.0)

# supress ZERO_BUILD
# set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory(NES)
add_subdirectory(SDL EXCLUDE_FROM_ALL)

add_executable(NES main.cpp)
target_link_libraries(NES PUBLIC LibNES)
target_link_libraries(NES PUBLIC SDL3::SDL3)
target_compile_definitions(NES PUBLIC SDL_MAIN_USE_CALLBACKS)
