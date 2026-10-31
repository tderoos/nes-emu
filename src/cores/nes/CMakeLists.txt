add_library(LibCoreNES 	
			APU.cpp
			APU.h
			CPU6502.cpp
			CPU6502.h
			IO.cpp
			IO.h
			Mapper.cpp
			Mapper.h
			Mappers/NROM.h
			Mappers/MMC1.h
			NES.cpp
			NES.h
			PPU2C07.cpp
			PPU2C07.h
			Ram.cpp
			Ram.h
			Rom.cpp 
			Rom.h
			SaveState.h)


target_include_directories(LibCoreNES PUBLIC ${CMAKE_SOURCE_DIR}/src)
