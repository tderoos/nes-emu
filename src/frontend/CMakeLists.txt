# The app: SDL frontend, target + packaging (bundle, icon, rpath).

add_executable(NES main.cpp)
target_include_directories(NES PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(NES PRIVATE LibCoreNES)
target_link_libraries(NES PRIVATE LibCartridge)
target_link_libraries(NES PRIVATE Common)
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
