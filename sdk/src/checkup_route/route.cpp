// https://router.hereapi.com/v8/routes?transportMode=car&origin=52.5308,13.3847&destination=52.5323,13.3789&return=summar

// Bento includes
#include <bento_base/log.h>

// SDK includes
#include "checkup_route/route.h"
#include "checkup_route/utility.h"

// External includes
#include <json.hpp>

namespace checkup
{
    void build_route_request(const char* originCoords, const char* destinationCoords, TransportMode mode, const char* token, TRequest& request)
    {
        request.validity = false;
        request.api = "https://router.hereapi.com/v8/routes";
        request.content = "transportMode=";
        request.content += transport_to_string(mode);
        request.content += "&origin=";
        request.content += originCoords;
        request.content += "&destination=";
        request.content += destinationCoords;
        request.content += "&return=summary,typicalDuration,polyline";
        request.content += "&apiKey=";
        request.content += token;
        request.result = "INVALID";
    }

    bool build_route_data(const bento::DynamicString& routeData, TRouteInfo& routeInfo, bento::ILogger* logger)
    {
        bool success = true;
        try
        {
            // Parse the json answer
            auto routeReponseJson = nlohmann::json::parse(routeData.c_str());
            
            auto routesJson = routeReponseJson["routes"][0];
            auto sectionsJson = routesJson["sections"][0];
            auto summaryJson = sectionsJson["summary"];
            routeInfo.duration = summaryJson["duration"];
            routeInfo.baseDuration = summaryJson["baseDuration"].get<uint32_t>();
            routeInfo.length = summaryJson["length"].get<uint32_t>() / 1000.0f;
            routeInfo.typicalDuration = summaryJson["typicalDuration"].get<uint32_t>();

            if (logger != nullptr)
            {
                logger->log(bento::LogLevel::info, "Duration", std::to_string(routeInfo.duration).c_str());
                logger->log(bento::LogLevel::info, "Length", std::to_string(routeInfo.length).c_str());
                logger->log(bento::LogLevel::info, "Typical Duration", std::to_string(routeInfo.typicalDuration).c_str());
                logger->log(bento::LogLevel::info, "Base Duration", std::to_string(routeInfo.baseDuration).c_str());
            }
            // Every data was fetch successfully
            success = true;
        }
        catch (nlohmann::json::exception&)
        {
            if (logger != nullptr)
                logger->log(bento::LogLevel::error, "Route", "Route parsing failed.");
            success = false;
        }
        return success;
    }
}
