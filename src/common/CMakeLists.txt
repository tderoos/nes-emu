add_library(Common
			Thread.cpp)
target_include_directories(Common PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(Common PUBLIC SDL3::SDL3)
