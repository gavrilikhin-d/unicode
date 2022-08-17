cmake_minimum_required(VERSION 3.19)

project(
	String
	VERSION "0.0.0"
	DESCRIPTION "Unicode string library"
	HOMEPAGE_URL https://github.com/gavrilikhin-d/String
	LANGUAGES CXX
)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)

if(MSVC)
	# warning level 4
	add_compile_options(/W4)
else()
	# lots of warnings
	add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(
		CMAKE_BUILD_TYPE "Debug" CACHE STRING
		"Debug | RelWithDebInfo | Release | MinSizeRel"
		FORCE
	)
	message(
		STATUS
		"No build type was specified, will default to ${CMAKE_BUILD_TYPE}"
	)
endif()

set(CMAKE_DISABLE_IN_SOURCE_BUILD YES)

# ICU dependency
find_package(ICU COMPONENTS uc REQUIRED)

add_library(String INTERFACE)
# default member initializer for bit-field is a C++20 extension
target_compile_features(String INTERFACE cxx_std_20)
target_link_libraries(String INTERFACE ${ICU_LIBRARIES})
target_include_directories(
	String INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR} 
		${ICU_INCLUDE_DIRS}
)

add_subdirectory(tests)
