// SDK includes
#include <bento_memory/system_allocator.h>
#include <bento_base/log.h>
#include <bento_resources/asset_database.h>
#include <bento_tools/disk_serializer.h>
#include <checkup_rest/session.h>
#include <checkup_weather/weather.h>
#include <checkup_weather/forecast.h>

// External includes
#include <fstream>
#include <string>
#include <json.hpp>

int main(int, char**)
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
    checkup::TRequest request(systemAllocator);
    for (uint32_t viewportIdx = 0; viewportIdx < viewportCount; ++viewportIdx)
    {
        // Grab the current viewport
        auto viewportJson = viewportArrayJson[viewportIdx];
        if (viewportJson["type"].get<std::string>() != "Weather")
            continue;

        const std::string& location = viewportJson["name"].get<std::string>();

        // Build and execute the weather request
        build_weather_request(location.c_str(), owmToken.c_str(), request);
        session.execute(request);
        checkup::TWeatherInfo weatherInfo(systemAllocator);
        build_weather_data(request.result, weatherInfo, logger);

        // Build and execute the forecast request
        build_forecast_request(weatherInfo.latitude, weatherInfo.longitude, owmToken.c_str(), request);
        session.execute(request);
        checkup::TForecastInfo forecastInfo(systemAllocator);
        build_forecast_data(request.result, forecastInfo, logger);
    }

    // Terminate the session
    session.terminate();

    // We are done!
    return 0;
}