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
#include <checkup_route/route.h>
#include <checkup_route/utility.h>
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

void run_rest_requests(TDisplayData& displayData, bento::IAllocator& allocator)
{
    // Create a session
    checkup::TSession session(allocator);

    // Init the session
    session.init();

    // Temporary request
    checkup::TRequest request(allocator);

    // Number of weather viewports to handle
    uint32_t numWeatherViewports = displayData.weatherViewportArray.size();
    displayData.weatherInfoArray.resize(numWeatherViewports);
    displayData.forecastInfoArray.resize(numWeatherViewports);

    // Loop throught the viewpots
    for (uint32_t weatherIdx = 0; weatherIdx < numWeatherViewports; ++weatherIdx)
    {
        // Grab the current viewport
        const checkup::TWeatherViewport& currentViewport = displayData.weatherViewportArray[weatherIdx];

        // Build and execute the weather request
        build_weather_request(currentViewport.cityName.c_str(), displayData.owmToken.c_str(), request);
        if (session.execute(request))
        {
            // build the weather data
            auto& curentWeatherInfo = displayData.weatherInfoArray[weatherIdx];
            build_weather_data(request.result, displayData.weatherInfoArray[weatherIdx]);

            // Build and execute the forecast request
            build_forecast_request(curentWeatherInfo.latitude, curentWeatherInfo.longitude, displayData.owmToken.c_str(), request);
            if (session.execute(request))
            {
                // Build the forecast data
                auto& curentForecastInfo = displayData.forecastInfoArray[weatherIdx];
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

    // Number of route viewports to handle
    uint32_t numRouteViewports = displayData.routeViewportArray.size();
    displayData.routeInfoArray.resize(numRouteViewports);

    // Loop throught the viewpots
    for (uint32_t routeIdx = 0; routeIdx < numRouteViewports; ++routeIdx)
    {
        // Grab the current viewport
        const checkup::TRouteViewport& currentViewport = displayData.routeViewportArray[routeIdx];

        checkup::TransportMode mode = checkup::string_to_transport(currentViewport.vehicle.c_str());
        build_route_request(currentViewport.origin.c_str(), currentViewport.destination.c_str(), mode, displayData.hereToken.c_str(), request);
        session.execute(request);

        // Parse the weather info
        auto& currentRoute = displayData.routeInfoArray[routeIdx];
        build_route_data(request.result, currentRoute);
    }

    // Number of ratp viewports to handle
    uint32_t numRatpViewports = displayData.ratpViewportArray.size();
    // Loop throught the viewpots
    if (numRatpViewports != 0)
    {
        build_ratp_request(request);
        session.execute(request);

        // Parse the weather info
        build_ratp_data(request.result, displayData.ratpInfo);
    }

    // Terminate the session
    session.terminate();
}

int main()
{
    // Create an allocator for our application
    bento::SystemAllocator systemAllocator;

    // Grab the logger
    bento::ILogger* logger = bento::default_logger();
    
    // Read the config file to a string
    std::ifstream istream("config.json");
    std::string str((std::istreambuf_iterator<char>(istream)), std::istreambuf_iterator<char>());

    // Parse the config
    nlohmann::json jsonConfig = nlohmann::json::parse(str.c_str());

    // The set of weather viewpots
    TDisplayData displayData(systemAllocator);

    // Grab the token
    displayData.owmToken = jsonConfig["tokens"]["open_weather_map"].get<std::string>().c_str();
    displayData.hereToken = jsonConfig["tokens"]["developer_here"].get<std::string>().c_str();

    // Grab the viewports
    auto viewportArrayJson = jsonConfig["viewports"];

    // Number of requests that need to be processed
    uint32_t viewportCount = (uint32_t)viewportArrayJson.size();

    // Loop through the viewports
    for (uint32_t viewportIdx = 0; viewportIdx < viewportCount; ++viewportIdx)
    {
        // Grab the current viewport
        auto viewportJson = viewportArrayJson[viewportIdx];
        if (viewportJson["type"].get<std::string>() == "Weather")
        {
            // Create new viewport
            checkup::TWeatherViewport& newWeatherViewport = displayData.weatherViewportArray.extend();
            newWeatherViewport.cityName = viewportJson["name"].get<std::string>().c_str();
            newWeatherViewport.startX = viewportJson["start_x"].get<float>();
            newWeatherViewport.startY = viewportJson["start_y"].get<float>();
            newWeatherViewport.width = viewportJson["size_x"].get<float>();
            newWeatherViewport.height = viewportJson["size_y"].get<float>();
            newWeatherViewport.debugColor.x = viewportJson["debug_color"]["r"].get<float>();
            newWeatherViewport.debugColor.y = viewportJson["debug_color"]["g"].get<float>();
            newWeatherViewport.debugColor.z = viewportJson["debug_color"]["b"].get<float>();
        }

        if (viewportJson["type"].get<std::string>() == "Route")
        {
            // Create new viewport
            checkup::TRouteViewport& routeViewport = displayData.routeViewportArray.extend();
            routeViewport.origin = viewportJson["origin"].get<std::string>().c_str();
            routeViewport.destination = viewportJson["destination"].get<std::string>().c_str();
            routeViewport.vehicle = viewportJson["vehicle"].get<std::string>().c_str();
            routeViewport.name = viewportJson["name"].get<std::string>().c_str();
            routeViewport.startX = viewportJson["start_x"].get<float>();
            routeViewport.startY = viewportJson["start_y"].get<float>();
            routeViewport.width = viewportJson["size_x"].get<float>();
            routeViewport.height = viewportJson["size_y"].get<float>();
            routeViewport.debugColor.x = viewportJson["debug_color"]["r"].get<float>();
            routeViewport.debugColor.y = viewportJson["debug_color"]["g"].get<float>();
            routeViewport.debugColor.z = viewportJson["debug_color"]["b"].get<float>();
        }

        if (viewportJson["type"].get<std::string>() == "RATP")
        {
            // Create new viewport
            checkup::TRATPViewport& ratpViewport = displayData.ratpViewportArray.extend();
            ratpViewport.name = viewportJson["name"].get<std::string>().c_str();
            ratpViewport.line = viewportJson["line"].get<std::string>().c_str();
            ratpViewport.startX = viewportJson["start_x"].get<float>();
            ratpViewport.startY = viewportJson["start_y"].get<float>();
            ratpViewport.width = viewportJson["size_x"].get<float>();
            ratpViewport.height = viewportJson["size_y"].get<float>();
            ratpViewport.debugColor.x = viewportJson["debug_color"]["r"].get<float>();
            ratpViewport.debugColor.y = viewportJson["debug_color"]["g"].get<float>();
            ratpViewport.debugColor.z = viewportJson["debug_color"]["b"].get<float>();
        }
    }

    // Run the rest questests
    run_rest_requests(displayData, systemAllocator);

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
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    // Create a gl context
    auto context = SDL_GL_CreateContext(window);

    // Create the renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Define the blend mode
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_ShowCursor(0);
    SDL_CaptureMouse(SDL_FALSE);

    // Build the UI
    ReadyToGoScreen* programUI = new ReadyToGoScreen(window, 1280, 720, displayData, systemAllocator);

    // Initial update time
    uint32_t lastUpdate = SDL_GetTicks();

    // Time to rerun the update (10 minutes)
    const uint32_t updateDuration = 1000 * 60 * 10;

    bool quit = false;
    SDL_Event e;
    while(!quit)
    {
        // If enough time has passed, we need to update everything
        if ((SDL_GetTicks() - lastUpdate) > updateDuration)
        {
            // Re-run the requets
            run_rest_requests(displayData, systemAllocator);

            // Delete the previous
            delete programUI;

            // Re-create the UI
            programUI = new ReadyToGoScreen(window, 1280, 720, displayData, systemAllocator);

            // Update the time
            lastUpdate = SDL_GetTicks();
        }

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
            programUI->onEvent(e);
        }
        
        // Clear the screen black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw the UI
        programUI->drawAll();

        // Render the rect to the screen
        SDL_RenderPresent(renderer);

        // Sleep 10 seconds
        SDL_Delay(10000);
    }
    
    return 0;
}
