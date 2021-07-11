#pragma once

// Base includes
#include <float.h>

// Bento includes
#include <bento_memory/common.h>
#include <bento_collection/dynamic_string.h>

// SDK includes
#include "checkup_weather/weather_category.h"

namespace checkup
{
    float kelvin_to_celcius(float kelvin);
    TWeatherCategory string_to_category(const char* category);
    const char* category_to_string(TWeatherCategory category);
}