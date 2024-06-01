cmake_minimum_required(VERSION 3.10)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

project(Nestor VERSION 1.0)

# supress ZERO_BUILD
set(CMAKE_SUPPRESS_REGENERATION true)

add_subdirectory(nestor)
add_subdirectory(SDL EXCLUDE_FROM_ALL)

add_executable(Nestor main.cpp)
target_link_libraries(Nestor PUBLIC LibNestor)
target_link_libraries(Nestor PUBLIC SDL3::SDL3)
target_compile_definitions(Nestor PUBLIC SDL_MAIN_USE_CALLBACKS)
