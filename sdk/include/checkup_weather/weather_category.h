#pragma once

// Base includes
#include <float.h>

// Bento includes
#include <bento_memory/common.h>
#include <bento_collection/dynamic_string.h>

// SDK includes
#include "checkup_rest/request.h"

namespace checkup
{
    enum class TWeatherCategory
    {
        Clear,
        Clouds,
        Rain,
        Thunderstorm,
        Drizzle,
        Snow,
        Extreme,
        Mist,
        Smoke,
        Haze,
        Dust,
        Fog,
        Sand,
        Ash,
        Squall,
        Tornado,
        Invalid
    };
}