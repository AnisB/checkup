// Bento includes
#include <bento_base/log.h>

// SDK includes
#include "checkup_weather/weather.h"

// External includes
#include <json.hpp>

namespace checkup
{
    void build_weather_request(const char* cityName, const char* token, TRequest& request)
    {
        request.validity = false;
        request.api = "https://api.openweathermap.org/data/2.5/weather";
        request.content = "q=";
        request.content += cityName;
        request.content += "&appid=";
        request.content += token;
        request.result = "INVALID";
    }

    bool build_weather_data(const bento::DynamicString& weatherData, TWeatherInfo& weatherInfo, bento::ILogger* logger)
    {
        bool success = true;
        try
        {
            // Parse the json answer
            auto pingJsonResponse = nlohmann::json::parse(weatherData.c_str());

            weatherInfo.cityName = pingJsonResponse["name"].get<std::string>().c_str();
            weatherInfo.countryCode = pingJsonResponse["sys"]["country"].get<std::string>().c_str();
            
            auto coordResult = pingJsonResponse["coord"];
            weatherInfo.latitude = coordResult["lat"].get<float>();
            weatherInfo.longitude = coordResult["lon"].get<float>();

            auto weatherResult = pingJsonResponse["weather"][0];
            weatherInfo.category = string_to_category(weatherResult["main"].get<std::string>().c_str());
            weatherInfo.description = weatherResult["description"].get<std::string>().c_str();

            auto mainResult = pingJsonResponse["main"];
            weatherInfo.temperature = kelvin_to_celcius(mainResult["temp"].get<float>());
            weatherInfo.feltTemperature = kelvin_to_celcius(mainResult["feels_like"].get<float>());
            weatherInfo.minTemperature = kelvin_to_celcius(mainResult["temp_min"].get<float>());
            weatherInfo.maxTemperature = kelvin_to_celcius(mainResult["temp_max"].get<float>());
            weatherInfo.humidity = mainResult["humidity"].get<float>();
            
            if (logger != nullptr)
            {
                logger->log(bento::LogLevel::info, "Country", weatherInfo.countryCode.c_str());
                logger->log(bento::LogLevel::info, "City", weatherInfo.cityName.c_str());
                logger->log(bento::LogLevel::info, "Longitude", std::to_string(weatherInfo.longitude).c_str());
                logger->log(bento::LogLevel::info, "Lattitude", std::to_string(weatherInfo.latitude).c_str());
                logger->log(bento::LogLevel::info, "Weather", category_to_string(weatherInfo.category));
                logger->log(bento::LogLevel::info, "Weather Description", weatherInfo.description.c_str());
                logger->log(bento::LogLevel::info, "Temperature", std::to_string(weatherInfo.temperature).c_str());
                logger->log(bento::LogLevel::info, "Felt Temperature", std::to_string(weatherInfo.feltTemperature).c_str());
                logger->log(bento::LogLevel::info, "Min Temperature", std::to_string(weatherInfo.minTemperature).c_str());
                logger->log(bento::LogLevel::info, "Max Temperature", std::to_string(weatherInfo.maxTemperature).c_str());
                logger->log(bento::LogLevel::info, "Humidity", std::to_string(weatherInfo.humidity).c_str());
            }
            // Every data was fetch successfully
            success = true;
        }
        catch (nlohmann::json::exception&)
        {
            if (logger != nullptr)
                logger->log(bento::LogLevel::error, "Weather", "Weather parsing failed.");
            success = false;
        }
        return success;
    }
}
