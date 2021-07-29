cmake_minimum_required(VERSION 3.2)

bento_exe("allocator_tester" "applications" "allocator_tester.cpp" "${CHECKUP_SDK_INCLUDE};${CHECKUP_3RD_INCLUDE}")
target_link_libraries("allocator_tester" "bento_sdk")

bento_exe("weather_display" "applications" "weather_display.cpp" "${CHECKUP_SDK_INCLUDE};${CHECKUP_3RD_INCLUDE}")
target_link_libraries("weather_display" "checkup_sdk" "bento_sdk" "${CURL_LIBRARIES}")

bento_exe("route_display" "applications" "route_display.cpp" "${CHECKUP_SDK_INCLUDE};${CHECKUP_3RD_INCLUDE}")
target_link_libraries("route_display" "checkup_sdk" "bento_sdk" "${CURL_LIBRARIES}")

bento_exe("hourly_forecast_display" "applications" "hourly_forecast_display.cpp" "${CHECKUP_SDK_INCLUDE};${CHECKUP_3RD_INCLUDE}")
target_link_libraries("hourly_forecast_display" "checkup_sdk" "bento_sdk" "${CURL_LIBRARIES}")

bento_exe("clear_to_tint" "applications" "clear_to_tint.cpp" "${CHECKUP_SDK_INCLUDE};${CHECKUP_3RD_INCLUDE};${SDL2_INCLUDE_DIRS}")
target_link_libraries("clear_to_tint" "checkup_sdk" "bento_sdk" "${CURL_LIBRARIES}" "${SDL2_LIBRARIES}")

bento_exe("ratp_display" "applications" "ratp_display.cpp" "${CHECKUP_SDK_INCLUDE};${CHECKUP_3RD_INCLUDE}")
target_link_libraries("ratp_display" "checkup_sdk" "bento_sdk" "${CURL_LIBRARIES}")

bento_exe("ready_to_go" "applications" "ready_to_go.cpp;ready_to_go/ready_to_go_screen.cpp;ready_to_go/ready_to_go_screen.h" "${CHECKUP_SDK_INCLUDE};${CHECKUP_3RD_INCLUDE};${SDL2_INCLUDE_DIRS};${SDL_GUI_INCLUDE}")

if (PLATFORM_WINDOWS)
	target_link_libraries("ready_to_go" "checkup_sdk" "bento_sdk" "sdlgui" "${CURL_LIBRARIES}" "${SDL2_LIBRARIES}" "${SDLGUI_LIBRARIES}")
elseif (PLATFORM_LINUX)
	target_link_libraries("ready_to_go" "checkup_sdk" "bento_sdk" "sdlgui" "${CURL_LIBRARIES}" "${SDL2_LIBRARIES}" "${SDLGUI_LIBRARIES}" -lpthread -lSDL2_image -lSDL2_ttf)
endif ()

if( PLATFORM_WINDOWS )
	copy_dependencies(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
	copy_dependencies(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
elseif(PLATFORM_LINUX)
	copy_dependencies(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif ()