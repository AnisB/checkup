#pragma once

// Base includes
#include <float.h>

// Bento includes
#include <bento_base/log.h>
#include <bento_memory/common.h>
#include <bento_collection/dynamic_string.h>

// SDK includes
#include "checkup_weather/weather_category.h"
#include "checkup_weather/utility.h"

namespace checkup
{
    struct TWeatherInfo
    {
        ALLOCATOR_BASED;
        TWeatherInfo(bento::IAllocator& allocator)
        : _allocator(allocator)
        , cityName(allocator, "INVALID")
        , countryCode(allocator, "INVALID")
        , latitude(FLT_MAX)
        , longitude(FLT_MAX)
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
        float latitude;
        float longitude;
        float temperature;
        float feltTemperature;
        float minTemperature;
        float maxTemperature;
        float humidity;
        TWeatherCategory category;
        bento::DynamicString description;
        bento::IAllocator& _allocator;
    };

    void build_weather_request(const char* cityName, const char* token, TRequest& request);
    bool build_weather_data(const bento::DynamicString& weatherData, TWeatherInfo& weatherInfo, bento::ILogger* logger = nullptr);
}