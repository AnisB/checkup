// Bento includes
#include <bento_base/log.h>

// SDK includes
#include "checkup_weather/utility.h"

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
}
