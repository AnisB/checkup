#pragma once

// Base includes
#include <float.h>

// Bento includes
#include <bento_memory/common.h>
#include <bento_collection/dynamic_string.h>

namespace checkup
{
    struct TWeatherInfo
    {
        ALLOCATOR_BASED;
        TWeatherInfo(bento::IAllocator& allocator)
        : _allocator(allocator)
        , temperature(FLT_MAX)
        , minTemperature(FLT_MAX)
        , maxTemperature(-FLT_MAX)
        {
        }
        float temperature;
        float minTemperature;
        float maxTemperature;
        bento::IAllocator& _allocator;
    };

    // Parses a json string and fetches all the useful data
    bool build_weather_data(const bento::DynamicString& weatherData, TWeatherInfo& weatherInfo);
}