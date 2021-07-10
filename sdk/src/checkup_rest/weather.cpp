// SDK includes
#include <bento_base/log.h>
#include "checkup_rest/weather.h"

// External includes
#include <json.hpp>

namespace checkup
{
    TWeatherCategory string_to_category(const char* category)
    {
        if (strcmp(category, "Clear") == 0)
            return TWeatherCategory::Clear;
        if (strcmp(category, "Clouds") == 0)
            return TWeatherCategory::Clouds;
        if (strcmp(category, "Rain") == 0)
            return TWeatherCategory::Rain;
        if (strcmp(category, "Snow") == 0)
            return TWeatherCategory::Snow;
        if (strcmp(category, "Extreme") == 0)
            return TWeatherCategory::Extreme;
        return TWeatherCategory::Invalid;
    }

    const char* category_to_string(TWeatherCategory category)
    {
        switch (category)
        {
            case TWeatherCategory::Clear:
                return "Clear";
            case TWeatherCategory::Clouds:
                return "Clouds";
            case TWeatherCategory::Rain:
                return "Rain";
            case TWeatherCategory::Snow:
                return "Snow";
            case TWeatherCategory::Extreme:
                return "Extreme";
        }
        return "INVALID";
    }

    // Temperature conversion
    float kelvin_to_celcius(float kelvin)
    {
        return (kelvin - 273.15f);
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
