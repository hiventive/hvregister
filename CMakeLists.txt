# CMake
cmake_minimum_required(VERSION 3.1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(YAML)

# Load settings
yaml_parse("settings.yml" "HV")

option(ENABLE_CONAN "Enable Conan. This option is automatically set to ON if conanbuildinfo.cmake file exists." OFF)
option(ENABLE_GCOV "Enable code coverage with gcov" OFF)
option(BUILD_TESTS "Enable tests build" OFF)
option(BUILD_DOXYGEN "Build documentation" OFF)
set(CONAN_PROFILE "default" CACHE STRING "Conan profile to use. Default value: default")
set(CONAN_BUILD "missing" CACHE STRING "Conan dependencies build option. Default value: missing")

# Hiventive
set(HV_LIB_PREFIX hv)
set(HV_NAMESPACE HV::)

# Generator
if(CMAKE_EXTRA_GENERATOR)
	if(${CMAKE_EXTRA_GENERATOR} STREQUAL "Eclipse CDT4")
		set(CMAKE_CXX_COMPILER_ARG1 "-std=c++11")
	endif()
endif()

# Project
project(${HV_PROJECT_NAME}
		VERSION ${HV_PROJECT_VERSION}
		LANGUAGES CXX)
enable_language(C) # Fix CMake 3.1.0 (at least) thread issue
string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
string(REPLACE ${HV_LIB_PREFIX} "" PROJECT_NAME_LOWER_WP ${PROJECT_NAME_LOWER})

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
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang")
		if(NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
		    message(WARNING "Debug mode must be activated to monitor code coverage.
Activating debug mode...")
		    set(CMAKE_BUILD_TYPE "Debug")
		endif()
	    include(codecoverage) # Project must be defined before inclusion
	    APPEND_COVERAGE_COMPILER_FLAGS()
    else()
        message(FATAL_ERROR "Code coverage can only be activated with GCC or (Apple)Clang.")
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
set(HV_CONAN_AVAILABLE FALSE)
if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake")
	include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
	set(HV_CONAN_AVAILABLE TRUE) # See ENABLE_CONAN option description
else()
	if(ENABLE_CONAN)
		include(conan)
		conan_cmake_run(CONANFILE conanfile.py
				PROFILE ${CONAN_PROFILE}
				BUILD ${CONAN_BUILD}
				BUILD_TYPE "Release") # Profile will overwrite this value
		set(HV_CONAN_AVAILABLE TRUE)
	endif()
endif()
if(HV_CONAN_AVAILABLE)
	conan_basic_setup(KEEP_RPATHS)
endif()

# Dependencies
# ${PROJECT_NAME}_PROJECT_DEPENDENCIES format is : NAME VERSION REQUIRED CONFIG
list(APPEND ${PROJECT_NAME}_PROJECT_DEPENDENCIES "Threads REQUIRED")
list(APPEND ${PROJECT_NAME}_PROJECT_DEPENDENCIES "HVCommon REQUIRED CONFIG")
list(APPEND ${PROJECT_NAME}_PROJECT_DEPENDENCIES "HVModule REQUIRED CONFIG")
list(APPEND ${PROJECT_NAME}_PROJECT_DEPENDENCIES "HVCommunication REQUIRED CONFIG")
list(APPEND ${PROJECT_NAME}_PROJECT_DEPENDENCIES "SystemCLanguage 2.3.2 REQUIRED CONFIG")
if(BUILD_TESTS)
	list(APPEND ${PROJECT_NAME}_PROJECT_DEPENDENCIES "GTest 1.8.0 REQUIRED")
	# CMake FindGTest.cmake distributed with CMake defines targets GTest::GTest and GTest::Main
	# GTest >= 1.8.1 provides a CMake CONFIG and defines targets GTest::gtest and GTest::gtest_main
	# Provided CMake FindGTest.cmake defines targets GTest::gtest and GTest::gtest_main to unify names
	# Remove FindGTest.cmake after the switch to GTest >= 1.8.1
	if(MSVC)
		add_definitions(-DGTEST_HAS_TR1_TUPLE=0)
	endif()
endif()

# Dependencies export
foreach(PROJECT_DEPENDENCY ${${PROJECT_NAME}_PROJECT_DEPENDENCIES})
	string(REPLACE " " ";" PROJECT_DEPENDENCY_LIST ${PROJECT_DEPENDENCY})
	find_package(${PROJECT_DEPENDENCY_LIST})
	list(REMOVE_ITEM PROJECT_DEPENDENCY_LIST "REQUIRED")
	string(REPLACE ";" " " EXPORT_PROJECT_DEPENDENCY "${PROJECT_DEPENDENCY_LIST}")
	list(APPEND EXPORT_${PROJECT_NAME}_PROJECT_DEPENDENCIES ${EXPORT_PROJECT_DEPENDENCY})
endforeach()

# If Release/Debug mode is not set, force _ITERATOR_DEBUG_LEVEL as Conan builds packages in Release/Debug mode
if(NOT CMAKE_BUILD_TYPE)
	if(MSVC)
		add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)
	endif()
endif()

# Library
add_subdirectory(src)

# Tests
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
