// SDK includes
#include <bento_base/log.h>
#include "checkup_rest/weather_info.h"

// External includes
#include <json.hpp>

namespace checkup
{
    // Temperature conversion
    float kelvin_to_celcius(float kelvin)
    {
        return (kelvin - 273.15f);
    }

    bool build_weather_data(const bento::DynamicString& weatherData, TWeatherInfo& weatherInfo)
    {
        bool success = true;
        try
        {
            // Parse the json answer
            auto pingJsonResponse = nlohmann::json::parse(weatherData.c_str());

            // Grab the main result
            auto mainResult = pingJsonResponse["main"];

            // Fill the temperature data
            weatherInfo.temperature = kelvin_to_celcius(mainResult["temp"].get<float>());
            weatherInfo.minTemperature = kelvin_to_celcius(mainResult["temp_min"].get<float>());
            weatherInfo.maxTemperature = kelvin_to_celcius(mainResult["temp_max"].get<float>());
            success = true;
        }
        catch (nlohmann::json::exception&)
        {
            bento::ILogger* logger = bento::default_logger();
            logger->log(bento::LogLevel::error, "MARKET", "Weather parsing failed.");
            success = false;
        }
        return success;
    }
}
