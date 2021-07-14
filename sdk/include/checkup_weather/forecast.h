#pragma once

// Bento includes
#include <bento_base/log.h>
#include <bento_memory/common.h>
#include <bento_collection/dynamic_string.h>

// SDK includes
#include "checkup_weather/weather_category.h"
#include "checkup_weather/utility.h"

// External includes
#include <float.h>

namespace checkup
{
    struct TCurrentForecastState
    {
        ALLOCATOR_BASED;
        TCurrentForecastState(bento::IAllocator& allocator)
            : _allocator(allocator)
            , time(0)
            , sunrise(0)
            , sunset(0)
            , temperature(FLT_MAX)
            , feltTemperature(FLT_MAX)
            , pressure(UINT32_MAX)
            , humidity(UINT32_MAX)
            , clouds(FLT_MAX)
            , windSpeed(FLT_MAX)
            , weatherCategory(TWeatherCategory::Invalid)
            , weatherDescription(allocator, "INVALID")
            , rain1h_o(0)
            , snow1h_o(0)
            , pop(FLT_MAX)
        {
        }

        uint32_t time;
        uint32_t sunrise;
        uint32_t sunset;
        float temperature;
        float feltTemperature;
        uint32_t pressure;
        uint32_t humidity;
        float clouds;
        float windSpeed;
        float rain1h_o;
        float snow1h_o;
        float pop;

        TWeatherCategory weatherCategory;
        bento::DynamicString weatherDescription;

        bento::IAllocator& _allocator;
    };

    struct THourlyForecastState
    {
        ALLOCATOR_BASED;
        THourlyForecastState(bento::IAllocator& allocator)
            : _allocator(allocator)
            , time(0)
            , temperature(FLT_MAX)
            , feltTemperature(FLT_MAX)
            , pressure(UINT32_MAX)
            , humidity(UINT32_MAX)
            , clouds(FLT_MAX)
            , windSpeed(FLT_MAX)
            , weatherCategory(TWeatherCategory::Invalid)
            , weatherDescription(allocator, "INVALID")
            , rain1h_o(0)
            , snow1h_o(0)
            , pop(FLT_MAX)
        {
        }

        uint32_t time;
        float temperature;
        float feltTemperature;
        uint32_t pressure;
        uint32_t humidity;
        float clouds;
        float windSpeed;
        float pop;
        float rain1h_o;
        float snow1h_o;

        TWeatherCategory weatherCategory;
        bento::DynamicString weatherDescription;

        bento::IAllocator& _allocator;
    };

    struct TDailyForecastState
    {
        ALLOCATOR_BASED;
        TDailyForecastState(bento::IAllocator& allocator)
            : _allocator(allocator)
            , temperature(FLT_MAX)
            , feltTemperature(FLT_MAX)
            , weatherCategory(TWeatherCategory::Invalid)
            , weatherDescription(allocator, "INVALID")
            , pop(FLT_MAX)
        {
        }

        float temperature;
        float feltTemperature;
        float pop;
        TWeatherCategory weatherCategory;
        bento::DynamicString weatherDescription;

        bento::IAllocator& _allocator;
    };

    struct TForecastInfo
    {
        ALLOCATOR_BASED;
        TForecastInfo(bento::IAllocator& allocator)
        : _allocator(allocator)
        , currentForecast(allocator)
        , hourlyForecast(allocator)
        , dailyForecast(allocator)
        {
        }

        uint32_t timeOffset;
        TCurrentForecastState currentForecast;
        bento::Vector<THourlyForecastState> hourlyForecast;
        bento::Vector<TDailyForecastState> dailyForecast;
        bento::IAllocator& _allocator;
    };

    void build_forecast_request(float lat, float lon, const char* token, TRequest& request);
    bool build_forecast_data(const bento::DynamicString& forecastData, TForecastInfo& forecastInfo, bento::ILogger* logger = nullptr);
}