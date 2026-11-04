# SDL frontend: application class

add_library(Frontend
			Frontend.cpp
			Frontend.h)
target_include_directories(Frontend PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(Frontend PUBLIC SDL3::SDL3)
target_link_libraries(Frontend PRIVATE LibCoreNES)
target_link_libraries(Frontend PRIVATE LibCartridge)
target_link_libraries(Frontend PRIVATE Common)

# Web frontend (Emscripten only): single-threaded Frontend subclass
if(EMSCRIPTEN)
	target_sources(Frontend PRIVATE WebFrontend.cpp WebFrontend.h)
endif()
