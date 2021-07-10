#pragma once

// Base includes
#include <float.h>

// Bento includes
#include <bento_memory/common.h>
#include <bento_collection/dynamic_string.h>

namespace checkup
{
    enum class TWeatherCategory
    {
        Clear,
        Clouds,
        Rain,
        Snow,
        Extreme,
        Invalid
    };

    struct TWeatherInfo
    {
        ALLOCATOR_BASED;
        TWeatherInfo(bento::IAllocator& allocator)
        : _allocator(allocator)
        , cityName(allocator, "INVALID")
        , countryCode(allocator, "INVALID")
        , temperature(FLT_MAX)
        , feltTemperature(FLT_MAX)
        , minTemperature(FLT_MAX)
        , maxTemperature(-FLT_MAX)
        , humidity(0)
        , category(TWeatherCategory::Invalid)
        , description(allocator, "INVALID")
        {
        }

        bento::DynamicString cityName;
        bento::DynamicString countryCode;
        float temperature;
        float feltTemperature;
        float minTemperature;
        float maxTemperature;
        float humidity;
        TWeatherCategory category;
        bento::DynamicString description;
        bento::IAllocator& _allocator;
    };

    float kelvin_to_celcius(float kelvin);
    TWeatherCategory string_to_category(char* category);
    bool build_weather_data(const bento::DynamicString& weatherData, TWeatherInfo& weatherInfo, bento::ILogger* logger = nullptr);
}