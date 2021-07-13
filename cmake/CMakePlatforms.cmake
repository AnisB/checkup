cmake_minimum_required(VERSION 3.5)

# This module is shared; use include blocker.
if( _PLATFORMS_ )
	return()
endif()

# Mark it as processed
set(_PLATFORMS_ 1)

include(CMakeCompiler)

# Detect target platform
if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
	set(PLATFORM_WINDOWS 1)
	set(PLATFORM_NAME "windows")
	add_definitions(-DWINDOWSPC)
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
	set(PLATFORM_LINUX 1)
	set(PLATFORM_NAME "linux")
	add_definitions(-DLINUXPC)
endif()

message(STATUS "Detected platform: ${PLATFORM_NAME}")

# Detect target architecture
if(PLATFORM_WINDOWS AND CMAKE_CL_64)
	set(PLATFORM_64BIT 1)
endif()

# Configure CMake global variables
set(CMAKE_INSTALL_MESSAGE LAZY)
# Set the output folders based on the identifier
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIRECTORY}/lib/${PROJECT_PLATFORM_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIRECTORY}/lib/${PROJECT_PLATFORM_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIRECTORY}/bin/${PROJECT_PLATFORM_NAME})

if( PLATFORM_WINDOWS )
	set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)
endif()

if( PLATFORM_WINDOWS )
	# CURL LIBRARY
	set(CURL_INCLUDE_DIRS ${CHECKUP_3RD_INCLUDE})
	set(CURL_LIBRARIES "${CHECKUP_3RD_LIB}/libcurl.lib")
	set(CURL_BINARIES "${CHECKUP_3RD_BIN}/libcurl.dll")

	# SDL LIBRARY
	set(SDL2_INCLUDE_DIRS "${CHECKUP_3RD_INCLUDE}")
	set(SDL2_LIBRARIES "${CHECKUP_3RD_LIB}/SDL2.lib;${CHECKUP_3RD_LIB}/SDL2_image.lib;${CHECKUP_3RD_LIB}/SDL2main.lib;${CHECKUP_3RD_LIB}/SDL2_ttf.lib;")
elseif(PLATFORM_LINUX)
	# CURL LIBRARY
	find_package(CURL REQUIRED)

	# SDL Library
	find_package(SDL2 REQUIRED)
endif()
