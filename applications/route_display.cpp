// SDK includes
#include <bento_base/log.h>
#include <bento_memory/system_allocator.h>
#include <bento_resources/asset_database.h>
#include <bento_tools/disk_serializer.h>
#include <bento_tools/file_system.h>

#include <checkup_rest/session.h>
#include <checkup_route/route.h>

// External includes
#include <string>
#include <fstream>
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
    const std::string& hereToken = jsonConfig["tokens"]["developer_here"].get<std::string>();

    // Grab the viewports
    auto viewportArrayJson = jsonConfig["viewports"];

    // Number of requests that need to be processed
    uint32_t viewportCount = (uint32_t)viewportArrayJson.size();
    checkup::TRequest request(systemAllocator);
    checkup::TRouteInfo routeInfo(systemAllocator);
    for (uint32_t viewportIdx = 0; viewportIdx < viewportCount; ++viewportIdx)
    {
        // Grab the current viewport
        auto viewportJson = viewportArrayJson[viewportIdx];
        if (viewportJson["type"].get<std::string>() != "Route")
            continue;

        const std::string& origin = viewportJson["origin"].get<std::string>();
        const std::string& destination = viewportJson["destination"].get<std::string>();
        build_route_request(origin.c_str(), destination.c_str(), checkup::TransportMode::Car, hereToken.c_str(), request);
        session.execute(request);

        // Parse the weather info
        logger->log(bento::LogLevel::info, "ROUTE", "Car");
        build_route_data(request.result, routeInfo, logger);

        build_route_request(origin.c_str(), destination.c_str(), checkup::TransportMode::Bicycle, hereToken.c_str(), request);
        session.execute(request);

        // Parse the weather info
        logger->log(bento::LogLevel::info, "ROUTE", "Bicyle");
        build_route_data(request.result, routeInfo, logger);
    }

    // Terminate the session
    session.terminate();

    // We are done!
    return 0;
}