// SDK includes
#include <bento_base/log.h>
#include <bento_memory/system_allocator.h>
#include <bento_resources/asset_database.h>
#include <bento_tools/disk_serializer.h>
#include <bento_tools/file_system.h>

#include <checkup_rest/session.h>
#include <checkup_weather/weather.h>

// External includes
#include <string>
#include <fstream>
#include <json.hpp>

int main(int argc, char** argv)
{
    // Check that the id is there
    if (argc != 2)
        return -1;

    // Create an allocator for our application
    bento::SystemAllocator systemAllocator;

    // Create a session
    checkup::TSession session(systemAllocator);

    // Init the session
    session.init();

    // Grab the logger
    bento::ILogger* logger = bento::default_logger();
    
    // Read the config file to a string
    std::ifstream istream(argv[1]);
    std::string str((std::istreambuf_iterator<char>(istream)), std::istreambuf_iterator<char>());

    // Parse the config
    nlohmann::json jsonConfig = nlohmann::json::parse(str.c_str());

    // Grab the token
    const std::string& owmToken = jsonConfig["open_weather_map"]["token"].get<std::string>();

    // Grab the locations
    auto locationsJson = jsonConfig["cities"];

    // Number of requests that need to be processed
    uint32_t requestCount = (uint32_t)locationsJson.size();
    checkup::TRequest request(systemAllocator);
    for (uint32_t requestIdx = 0; requestIdx < requestCount; ++requestIdx)
    {
        // Build and execute the request
        const std::string& location = locationsJson[requestIdx]["name"].get<std::string>();
        build_weather_request(location.c_str(), owmToken.c_str(), request);
        session.execute(request);

        // Parse the weather info
        checkup::TWeatherInfo weatherInfo(systemAllocator);
        build_weather_data(request.result, weatherInfo, logger);
    }

    // Terminate the session
    session.terminate();

    // We are done!
    return 0;
}