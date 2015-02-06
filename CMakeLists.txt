cmake_minimum_required (VERSION 2.8.11)
project (MondX)

if (NOT WIN32)
	set(CMAKE_CXX_FLAGS "-std=c++11")
else()
	set(CMAKE_CXX_FLAGS "/EHsc")
endif()

add_subdirectory (MondX)
add_subdirectory (MondLint)
