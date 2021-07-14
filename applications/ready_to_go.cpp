// External includes
#include <fstream>
#include <string>
#include <json.hpp>
#include <memory>

// SDK includes
#include <bento_base/log.h>
#include <bento_base/platform.h>
#include <bento_memory/system_allocator.h>
#include <bento_resources/asset_database.h>
#include <bento_tools/disk_serializer.h>
#include <checkup_rest/session.h>
#include <checkup_weather/utility.h>
#include <checkup_weather/weather.h>
#include <checkup_weather/forecast.h>
#include "ready_to_go/ready_to_go_screen.h"

#if defined(_WIN32)
#include <windows.h>
#endif
#include <iostream>

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#if defined(_WIN32)
#include <SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

#undef main

int main()
{
    // Create an allocator for our application
    bento::SystemAllocator systemAllocator;

    // Create a session
    checkup::TSession session(systemAllocator);

    // Init the session
    session.init();

    // Grab the logger
    bento::ILogger* logger = bento::default_logger();
    
    // Read the config file to a string
    std::ifstream istream("config.json");
    std::string str((std::istreambuf_iterator<char>(istream)), std::istreambuf_iterator<char>());

    // Parse the config
    nlohmann::json jsonConfig = nlohmann::json::parse(str.c_str());

    // Grab the token
    const std::string& owmToken = jsonConfig["tokens"]["open_weather_map"].get<std::string>();

    // Grab the viewports
    auto viewportArrayJson = jsonConfig["viewports"];

    // Number of requests that need to be processed
    uint32_t viewportCount = (uint32_t)viewportArrayJson.size();

    // The set of weather viewpots
    bento::Vector<checkup::TWeatherViewport> weatherViewportArray(systemAllocator);

    // Loop through the viewports
    for (uint32_t viewportIdx = 0; viewportIdx < viewportCount; ++viewportIdx)
    {
        // Grab the current viewport
        auto viewportJson = viewportArrayJson[viewportIdx];
        if (viewportJson["type"].get<std::string>() != "Weather")
            continue;
        
        // Create new viewport
        checkup::TWeatherViewport& newWeatherViewport = weatherViewportArray.extend();
        newWeatherViewport.cityName = viewportJson["name"].get<std::string>().c_str();
        newWeatherViewport.startX = viewportJson["start_x"].get<float>();
        newWeatherViewport.startY = viewportJson["start_y"].get<float>();
        newWeatherViewport.width = viewportJson["size_x"].get<float>();
        newWeatherViewport.height = viewportJson["size_y"].get<float>();
        newWeatherViewport.debugColor.x = viewportJson["debug_color"]["r"].get<float>();
        newWeatherViewport.debugColor.y = viewportJson["debug_color"]["g"].get<float>();
        newWeatherViewport.debugColor.z = viewportJson["debug_color"]["b"].get<float>();
    }

    // Number of viewports to handle
    uint32_t numWeatherViewports = weatherViewportArray.size();

    // Allocate the structures that will be updates
    checkup::TRequest request(systemAllocator);
    bento::Vector<checkup::TWeatherInfo> weatherInfoArray(systemAllocator, numWeatherViewports);
    bento::Vector<checkup::TForecastInfo> forecastInfoArray(systemAllocator, numWeatherViewports);

    // Loop throught the viewpots
    for (uint32_t weatherIdx = 0; weatherIdx < numWeatherViewports; ++weatherIdx)
    {
        // Grab the current viewport
        const checkup::TWeatherViewport& currentViewport = weatherViewportArray[weatherIdx];
        
        // Build and execute the weather request
        build_weather_request(currentViewport.cityName.c_str(), owmToken.c_str(), request);
        if (session.execute(request))
        {
            // build the weather data
            auto& curentWeatherInfo = weatherInfoArray[weatherIdx];
            build_weather_data(request.result, weatherInfoArray[weatherIdx]);

            // Build and execute the forecast request
            build_forecast_request(curentWeatherInfo.latitude, curentWeatherInfo.longitude, owmToken.c_str(), request);
            if (session.execute(request))
            {
                // Build the forecast data
                auto& curentForecastInfo = forecastInfoArray[weatherIdx];
                build_forecast_data(request.result, curentForecastInfo);
            }
            else
            {
                bento::default_logger()->log(bento::LogLevel::error, "Invalid Forecast", currentViewport.cityName.c_str());
            }
        }
        else
        {
            bento::default_logger()->log(bento::LogLevel::error, "Invalid Forecast", currentViewport.cityName.c_str());
        }
    }

    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        bento::default_logger()->log(bento::LogLevel::error, "SDL Init", SDL_GetError());

    // ENable the attributes that we need
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Create window
    SDL_Window* window = SDL_CreateWindow("Ready To Go", 0, 0, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    // Check that the window was successfully made
    if(window == nullptr)
        bento::default_logger()->log(bento::LogLevel::error, "SDL Window", SDL_GetError());

    // Set the window full screen
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    // Create a gl context
    auto context = SDL_GL_CreateContext(window);

    // Create the renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Define the blend mode
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_ShowCursor(0);
    SDL_CaptureMouse(SDL_FALSE);

    // Build the UI
    ReadyToGoScreen programUI(window, 1280, 720, weatherViewportArray, weatherInfoArray, forecastInfoArray, systemAllocator);

    bool quit = false;
    SDL_Event e;
    while(!quit)
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            // User requests quit
            if( e.type == SDL_QUIT )
                quit = true;

            // We intentionally ignore mouse motions
            if (e.type == SDL_MOUSEMOTION)
                continue;

            // Inject the events to the UI
            programUI.onEvent(e);
        }
        
        // Clear the screen black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw the UI
        programUI.drawAll();

        // Render the rect to the screen
        SDL_RenderPresent(renderer);

        // Sleep 300 miliseconds
        SLEEP_FUNCTION(300);
    }

    // Terminate the session
    session.terminate();
    
    return 0;
}
