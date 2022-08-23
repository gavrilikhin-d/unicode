cmake_minimum_required(VERSION 3.19)

project(
	unicode
	VERSION "1.0.0"
	DESCRIPTION "Unicode related library"
	HOMEPAGE_URL https://github.com/gavrilikhin-d/unicode
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
	set_property(
		CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS 
		Debug RelWithDebInfo Release MinSizeRel
	)
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

# Print out path and version of any installed commands
message(STATUS "CMake (${CMAKE_COMMAND}) Version: ${CMAKE_VERSION}")

if(XCODE)
	set(version_flag -version)
else()
	set(version_flag --version)
endif()

execute_process(
	COMMAND ${CMAKE_MAKE_PROGRAM} ${version_flag}
	OUTPUT_VARIABLE _CMAKE_MAKE_PROGRAM_VERSION
	OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(
	STATUS
	"CMake Make Program (${CMAKE_MAKE_PROGRAM}) Version: 
	${_CMAKE_MAKE_PROGRAM_VERSION}"
)
message(
	STATUS
	"C Compiler (${CMAKE_C_COMPILER}) Version: ${CMAKE_C_COMPILER_VERSION}"
)
message(
	STATUS
	"C++ Compiler (${CMAKE_CXX_COMPILER}) Version: 
	${CMAKE_CXX_COMPILER_VERSION}"
)
message(STATUS "")

set(CMAKE_DISABLE_IN_SOURCE_BUILD YES)

# ICU dependency
find_package(ICU 70 COMPONENTS uc i18n REQUIRED)
while (NOT EXISTS ${ICU_INCLUDE_DIRS}/unicode/brkiter.h)
	# TODO: fallback to other package
	message(FATAL_ERROR "ICU include directory doesn't contain brkiter.h")
endwhile()

include_directories(include)
add_subdirectory(sources)

add_subdirectory(tests)
add_subdirectory(benchmarks)
