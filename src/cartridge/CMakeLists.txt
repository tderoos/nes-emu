# Platform-agnostic cartridge loading: file to memory + platform type
# detection. The frontend uses this to decide which core to run;
# cores consume the data. Neither knows about file formats beyond
# what CartridgeData exposes.
add_library(LibCartridge
			CartridgeData.cpp
			CartridgeData.h)
target_include_directories(LibCartridge PUBLIC ${CMAKE_SOURCE_DIR}/src)
