# CMake
cmake_minimum_required(VERSION 3.1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(${CMAKE_CURRENT_SOURCE_DIR}/conanbuildinfo.cmake)

# Load settings
file(READ "settings.yml" _PROJECT_SETTINGS)
string(REGEX MATCH "version: ([0-9]+)\\.([0-9]+)\\.([0-9]+)" _PROJECT_VERSION_LINE ${_PROJECT_SETTINGS})
string(REGEX MATCH "([0-9]+)\\.([0-9]+)\\.([0-9]+)" _PROJECT_VERSION ${_PROJECT_VERSION_LINE})
string(REGEX MATCH "name: [0-9_a-zA-Z]+" _PROJECT_NAME_LINE ${_PROJECT_SETTINGS})
string(REGEX MATCH "\ [0-9_a-zA-Z]+" _PROJECT_NAME ${_PROJECT_NAME_LINE})
unset(_PROJECT_SETTINGS)
unset(_PROJECT_VERSION_LINE)
unset(_PROJECT_NAME_LINE)

option(ENABLE_GCOV "Enable code coverage with gcov" OFF)
option(BUILD_TESTS "Enable tests build" OFF)
option(BUILD_DOXYGEN "Build documentation" OFF)

# Generator
if(CMAKE_EXTRA_GENERATOR)
	if(${CMAKE_EXTRA_GENERATOR} STREQUAL "Eclipse CDT4")
		set(CMAKE_CXX_COMPILER_ARG1 "-std=c++11")
	endif()
endif()

# Project
project(Register
		VERSION ${_PROJECT_VERSION}
		LANGUAGES CXX)
enable_language(C) # Fix CMake 3.1.0 (at least) thread issue
string(TOLOWER ${CMAKE_PROJECT_NAME} CMAKE_PROJECT_NAME_LOWER)
unset(_PROJECT_VERSION)
unset(_PROJECT_NAME)

# CXX
if(MSVC)
	if(NOT CMAKE_BUILD_TYPE)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")
	endif()
	if(MSVC_VERSION LESS 1700)
		message(FATAL_ERROR "Microsoft Visual Studio 2012 is required.")
	elseif(MSVC_VERSION EQUAL 1700)
		message("Microsoft Visual C++ Compiler Nov 2012 CTP (v120_CTP_Nov2012) is required.")
		set(CMAKE_GENERATOR_TOOLSET "v120_CTP_Nov2012")
	elseif(MSVC_VERSION EQUAL 1800)
		message("Microsoft Visual C++ Compiler Nov 2013 CTP (CTP_Nov2013) is required.")
		set(CMAKE_GENERATOR_TOOLSET "CTP_Nov2013")
	endif()
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
endif()
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# Thread
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

# Code coverage
if(ENABLE_GCOV)
	if(CMAKE_COMPILER_IS_GNUCXX)
		if(NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
			message(WARNING "Debug mode must be activated to monitor code coverage.
Activating debug mode...")
			set(CMAKE_BUILD_TYPE "Debug")
		endif()
		include(codecoverage) # Project must be defined before inclusion
		APPEND_COVERAGE_COMPILER_FLAGS()
	else()
		message(FATAL_ERROR "Code coverage can only be activated with GCC.")
	endif()
endif()

# Doxygen
if(BUILD_DOXYGEN)
	find_package(Doxygen)
	set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
	set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

	configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

	add_custom_target(doxygen ALL
			COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Generating documentation with Doxygen"
			VERBATIM)
endif()

# Conan
conan_basic_setup(KEEP_RPATHS)

# Dependencies
find_package(Threads REQUIRED)
if(MSVC AND MSVC_VERSION EQUAL 1700)
	message(WARNING "SPDLog is unsupported with VS 2012.")
else()
	find_package(spdlog 0.16.2 REQUIRED CONFIG)
endif()
find_package(SystemCLanguage 2.3.2 REQUIRED CONFIG)
find_package(Threads REQUIRED)
if(BUILD_TESTS)
	find_package(GTest REQUIRED)
	if(MSVC)
		add_definitions(-DGTEST_HAS_TR1_TUPLE=0)
	endif()
endif()

# If Release/Debug mode is not set, force _ITERATOR_DEBUG_LEVEL as Conan builds packages in Release/Debug mode
if(NOT CMAKE_BUILD_TYPE)
	if(MSVC)
		add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
	endif()
endif()

# Library
include_directories(src)
add_subdirectory(src)

# Tests
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
