cmake_minimum_required(VERSION 3.2)

bento_exe("weather_display" "applications" "weather_display.cpp" "${CHECKUP_SDK_INCLUDE};${CHECKUP_3RD_INCLUDE}")
target_link_libraries("weather_display" "checkup_sdk" "bento_sdk" "${CURL_LIBRARIES}")

bento_exe("hourly_forecast_display" "applications" "hourly_forecast_display.cpp" "${CHECKUP_SDK_INCLUDE};${CHECKUP_3RD_INCLUDE}")
target_link_libraries("hourly_forecast_display" "checkup_sdk" "bento_sdk" "${CURL_LIBRARIES}")

if( PLATFORM_WINDOWS )
	copy_dependencies(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
	copy_dependencies(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
elseif(PLATFORM_LINUX)
	copy_dependencies(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif ()
