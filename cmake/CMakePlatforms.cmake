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

	# EGL LIBRARY
	set(EGL_INCLUDE_DIRS ${CHECKUP_3RD_INCLUDE})
	set(EGL_LIBRARIES "${CHECKUP_3RD_LIB}/libEGL.lib")
	set(EGL_BINARIES "${CHECKUP_3RD_BIN}/libEGL.dll")

	# OPENGLES LIBRARY
	set(GLES2_INCLUDE_DIRS ${CHECKUP_3RD_INCLUDE})
	set(GLES2_LIBRARIES "${CHECKUP_3RD_LIB}/libGLESv2.lib")
	set(GLES2_BINARIES "${CHECKUP_3RD_BIN}/libGLESv2.dll")

	# MALI Emulator LIBRARY
	set(MALI_INCLUDE_DIRS ${CHECKUP_3RD_INCLUDE})
	set(MALI_LIBRARIES "${CHECKUP_3RD_LIB}/libMaliEmulator.lib")
	set(MALI_BINARIES "${CHECKUP_3RD_BIN}/libMaliEmulator.dll")

	set(LOG4CPLUS_BINARIES "${CHECKUP_3RD_BIN}/log4cplus.dll")
elseif(PLATFORM_LINUX)
	# CURL LIBRARY
	find_package(CURL REQUIRED)

	# X11 LIBRARY
	find_package(X11 REQUIRED)

	# ADDITIONAL LIBRARIES
	set(GRAPHICS_LIBRARIES ${X11_LIBRARIES})

	# EGL LIBRARY
	set(EGL_INCLUDE_DIRS ${CHECKUP_3RD_INCLUDE})
	set(EGL_LIBRARIES "${CHECKUP_3RD_LIB}/libEGL.so.1.4.0")
	set(EGL_BINARIES "${CHECKUP_3RD_LIB}/libEGL.so.1.4.0")

	# OPENGLES LIBRARY
	set(GLES2_INCLUDE_DIRS ${CHECKUP_3RD_INCLUDE})
	set(GLES2_LIBRARIES "${CHECKUP_3RD_LIB}/libGLESv2.so.3.2.0")
	set(GLES2_BINARIES "${CHECKUP_3RD_LIB}/libGLESv2.so.3")

	# MALI Emulator LIBRARY
	set(MALI_INCLUDE_DIRS ${CHECKUP_3RD_INCLUDE})
	set(MALI_LIBRARIES "${CHECKUP_3RD_LIB}/libMaliEmulator.so.3.0.4-2-g8d905")
	set(MALI_BINARIES "${CHECKUP_3RD_LIB}/libMaliEmulator.so.3.0.4-2-g8d905")
	
	# LIBLOG
	set(LOG4CPLUS_LIBRARIES "${CHECKUP_3RD_LIB}/liblog4cplus.so.1.1.2")
	set(LOG4CPLUS_BINARIES "${CHECKUP_3RD_LIB}/liblog4cplus.so.1.1.2")
endif()
