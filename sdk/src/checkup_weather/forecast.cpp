// SDK includes
#include <bento_base/log.h>
#include <bento_memory/system_allocator.h>
#include "checkup_weather/forecast.h"

// External includes
#include <json.hpp>

namespace checkup
{
    void build_forecast_request(float lat, float lon, const char* token, TRequest& request)
    {
        request.validity = false;
        request.api = "https://api.openweathermap.org/data/2.5/onecall";
        request.content = "lat=";
        request.content += std::to_string(lat).c_str();
        request.content += "&lon=";
        request.content += std::to_string(lon).c_str();
        request.content += "&appid=";
        request.content += token;
        request.result = "INVALID";
    }

    void build_current_forecast_state_data(const nlohmann::json& graph, TCurrentForecastState& state)
    {
        auto weatherResult = graph["weather"][0];
        state.weatherCategory = string_to_category(weatherResult["main"].get<std::string>().c_str());
        state.weatherDescription = weatherResult["description"].get<std::string>().c_str();

        state.time = graph["dt"].get<uint32_t>();
        state.sunrise = graph["sunrise"].get<uint32_t>();
        state.sunset = graph["sunset"].get<uint32_t>();
        state.temperature = kelvin_to_celcius(graph["temp"].get<float>());
        state.feltTemperature = kelvin_to_celcius(graph["feels_like"].get<float>());
        state.pressure = graph["pressure"].get<uint32_t>();
        state.humidity = graph["humidity"].get<uint32_t>();
        state.clouds = graph["clouds"].get<float>();
        state.windSpeed = graph["wind_speed"].get<float>();

        if (graph.find("rain") != graph.end())
        {
            auto rainNode = graph["rain"];
            if (rainNode.find("1h") != rainNode.end())
                state.rain1h_o = rainNode["1h"].get<float>();
        }

        if (graph.find("snow") != graph.end())
        {
            auto snowNode = graph["snow"];
            if (snowNode.find("1h") != snowNode.end())
                state.snow1h_o = snowNode["1h"].get<float>();
        }
    }

    void build_hourly_forecast_state_data(const nlohmann::json& graph, THourlyForecastState& state)
    {
        auto weatherResult = graph["weather"][0];
        state.weatherCategory = string_to_category(weatherResult["main"].get<std::string>().c_str());
        state.weatherDescription = weatherResult["description"].get<std::string>().c_str();

        state.time = graph["dt"].get<uint32_t>();
        state.temperature = kelvin_to_celcius(graph["temp"].get<float>());
        state.feltTemperature = kelvin_to_celcius(graph["feels_like"].get<float>());
        state.pressure = graph["pressure"].get<uint32_t>();
        state.humidity = graph["humidity"].get<uint32_t>();
        state.clouds = graph["clouds"].get<float>();
        state.windSpeed = graph["wind_speed"].get<float>();

        if (graph.find("rain") != graph.end())
        {
            auto rainNode = graph["rain"];
            if (rainNode.find("1h") != rainNode.end())
                state.rain1h_o = rainNode["1h"].get<float>();
        }

        if (graph.find("snow") != graph.end())
        {
            auto snowNode = graph["snow"];
            if (snowNode.find("1h") != snowNode.end())
                state.snow1h_o = snowNode["1h"].get<float>();
        }

        state.pop = graph["pop"].get<float>();
    }

    bool build_forecast_data(const bento::DynamicString& forecastData, TForecastInfo& forecastInfo, bento::ILogger* logger)
    {
        bool success = true;
        try
        {
            // Parse the json answer
            auto jsonResponse = nlohmann::json::parse(forecastData.c_str());

            // Fill the current forecast
            build_current_forecast_state_data(jsonResponse["current"], forecastInfo.currentForecast);
            
            // Fill the hourly forecast
            auto hourly = jsonResponse["hourly"];
            uint32_t numHours = (uint32_t)hourly.size();
            forecastInfo.hourlyForecast.resize(numHours);
            for (uint32_t hourIdx = 0; hourIdx < numHours; ++hourIdx)
                build_hourly_forecast_state_data(hourly[hourIdx], forecastInfo.hourlyForecast[hourIdx]);

            if (logger != nullptr)
            {
                bento::SystemAllocator tempAllocator;
                bento::DynamicString precipitation(tempAllocator);
                for (uint32_t hourIdx = 0; hourIdx < numHours; ++hourIdx)
                {
                    const THourlyForecastState& state = forecastInfo.hourlyForecast[hourIdx];
                    precipitation += std::to_string(state.pop).c_str();
                    precipitation += " ";
                }
                logger->log(bento::LogLevel::info, "Hourly Precipitation", precipitation.c_str());
            }

            // Every data was fetch successfully
            success = true;
        }
        catch (nlohmann::json::exception&)
        {
            if (logger != nullptr)
                logger->log(bento::LogLevel::error, "Forecast", "Forecast parsing failed.");
            success = false;
        }
        return success;
    }
}
