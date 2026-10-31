# Our code: compile policy (warnings, LTO config) and all our targets.
# The root CMakeLists only wires this and extern/SDL together, so
# nothing below applies to vendor code.

# Warnings for all our targets, all configurations
add_compile_options(-Wall -Wextra -Wno-unused-parameter)

# Custom configuration with LTO enabled, for when maximum runtime
# speed matters. Plain Release stays debuggable (no LTO); use this
# profile to compare, or when shipping/playing. Add it in CLion:
# Settings > Build, Execution, Deployment > CMake > + > ReleaseLTO
set(CMAKE_CXX_FLAGS_RELEASELTO
	"-O2 -DNDEBUG"
	CACHE STRING "Flags used by the compiler during ReleaseLTO builds" FORCE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASELTO ON)

add_subdirectory(common)
add_subdirectory(cores/nes)

add_executable(NES frontend/main.cpp)
target_include_directories(NES PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(NES PUBLIC LibCoreNES)
target_link_libraries(NES PUBLIC Common)
target_link_libraries(NES PUBLIC SDL3::SDL3)
target_compile_definitions(NES PUBLIC SDL_MAIN_USE_CALLBACKS)
