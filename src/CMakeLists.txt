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

# On macOS, build a proper .app bundle. Running a bare executable
# makes macOS treat the process as a second-class citizen: AppKit
# registration with system services (Intents, WindowTab indexing)
# stalls or times out, which showed up as seconds of extra startup.
# A bundle with an identity also gets a Dock presence and stable
# Gatekeeper handling on rebuilds.
if(APPLE)
	set_target_properties(NES PROPERTIES
		MACOSX_BUNDLE				TRUE
		MACOSX_BUNDLE_BUNDLE_NAME		  "nes-emu"
		MACOSX_BUNDLE_GUI_IDENTIFIER	  "com.tderoos.nes-emu"
		MACOSX_BUNDLE_BUNDLE_VERSION	  "1.0"
		MACOSX_BUNDLE_LONG_VERSION_STRING "1.0"
		MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
		# BUNDLE_ICON_FILE: add nes-emu.icns to Resources to give the app an icon
	)

	# Find the SDL3 shared library next to the executable (build tree).
	# For distributing the bundle, copy SDL3 into Frameworks/ and add
	# that to the rpath - BundleUtilities handles that at install time.
	set_target_properties(NES PROPERTIES
		BUILD_WITH_INSTALL_RPATH		FALSE
		MACOSX_RPATH				TRUE
		INSTALL_RPATH				"@loader_path"
		BUILD_RPATH				"@loader_path")
endif()
