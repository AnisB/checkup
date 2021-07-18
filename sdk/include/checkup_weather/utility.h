#pragma once

// Bento includes 
#include <bento_math/types.h>
#include <bento_memory/common.h>
#include <bento_collection/dynamic_string.h>

// SDK includes
#include "checkup_weather/weather_category.h"
#include "checkup_weather/forecast.h"

// External includes
#include <float.h>

namespace checkup
{
    float kelvin_to_celcius(float kelvin);
    TWeatherCategory string_to_category(const char* category);
    const char* category_to_string(TWeatherCategory category);
    bento::Vector3 temperature_to_color(float temperature);
    bool is_day(uint32_t time, uint32_t sunrise, uint32_t sunset);
    void time_to_string(int64_t time, bento::DynamicString& outputString);
    void duration_to_string(uint32_t duration, bento::DynamicString& outputString);
}