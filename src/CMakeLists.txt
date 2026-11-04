# Our code: compile policy and wiring. Each component (common,
# cartridge, cores/*, frontend) owns exactly one target in its own
# CMakeLists; nothing references another directory's sources.
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
add_subdirectory(cartridge)
add_subdirectory(cores/nes)
add_subdirectory(frontend)

# The app: SDL app callbacks (main.cpp) + frontend library, with packaging
# (.app bundle on macOS, wasm + html shell under Emscripten).
add_executable(NES main.cpp)
target_include_directories(NES PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(NES PRIVATE Frontend LibCoreNES LibCartridge Common SDL3::SDL3)
target_compile_definitions(NES PUBLIC SDL_MAIN_USE_CALLBACKS)

if(EMSCRIPTEN)
	# Web build: the WebFrontend is single-threaded, so no
	# pthreads/SharedArrayBuffer and no COOP/COEP headers are needed -
	# the page runs on any static host.
	target_link_options(NES PRIVATE
		--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/frontend/web/shell.html
		-sENVIRONMENT=web
		-sFILESYSTEM=1
		-sALLOW_MEMORY_GROWTH=1
		-sINITIAL_MEMORY=33554432
		-sEXIT_RUNTIME=1)
	set_target_properties(NES PROPERTIES SUFFIX ".html")
endif()

# On macOS, build a proper .app bundle. Running a bare executable
# makes macOS treat the process as a second-class citizen: AppKit
# registration with system services (Intents, WindowTab indexing)
# stalls or times out, which showed up as seconds of extra startup.
# A bundle with an identity also gets a Dock presence and stable
# Gatekeeper handling on rebuilds.
if(APPLE)
	set_target_properties(NES PROPERTIES
		MACOSX_BUNDLE					TRUE
		MACOSX_BUNDLE_BUNDLE_NAME		  "nes-emu"
		MACOSX_BUNDLE_GUI_IDENTIFIER	  "com.tderoos.nes-emu"
		MACOSX_BUNDLE_BUNDLE_VERSION	  "1.0"
		MACOSX_BUNDLE_LONG_VERSION_STRING "1.0"
		MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
	)
	# App icon: see assets/ (IconSet.iconset -> iconutil -> .icns)
	set(APP_ICON "${CMAKE_SOURCE_DIR}/assets/nes-emu.icns")
	if(EXISTS "${APP_ICON}")
		set_target_properties(NES PROPERTIES MACOSX_BUNDLE_ICON_FILE nes-emu)
		target_sources(NES PRIVATE "${APP_ICON}")
		set_source_files_properties("${APP_ICON}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
	endif()

	# Find the SDL3 shared library next to the executable (build tree).
	# For distributing the bundle, copy SDL3 into Frameworks/ and add
	# that to the rpath - BundleUtilities handles that at install time.
	set_target_properties(NES PROPERTIES
		BUILD_WITH_INSTALL_RPATH		FALSE
		MACOSX_RPATH					TRUE
		INSTALL_RPATH					"@loader_path"
		BUILD_RPATH						"@loader_path")
endif()
