// Bento includes
#include <bento_base/log.h>

// SDK includes
#include "checkup_weather/utility.h"

// External includes
#include <sstream>
#include <iomanip>

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

    float c_clamp(float a, float min, float max)
    {
        float res = a < min ? min : a;
        res = res > max ? max : res;
        return res;
    }

    bool is_day(uint32_t time, uint32_t sunrise, uint32_t sunset)
    {
        return (time > sunrise)
            && (time < sunset);
    }

    bento::Vector3 temperature_to_color(float temperature)
    {
        bento::Vector3 outColor;
        if (temperature < 22.0f)
        {
            float lerpParam = c_clamp((temperature - 10.0f) / (22.0f - 10.0f), 0.0f, 1.0f);
            outColor.x = 0.0f;
            outColor.y = lerpParam;
            outColor.z = 1.0f - lerpParam;
        }
        else
        {
            float lerpParam = c_clamp((temperature - 22.0f) / (30.0f - 22.0f), 0.0f, 1.0f);
            outColor.x = lerpParam;
            outColor.y = 1.0f - lerpParam;
            outColor.z = 0.0f;
        }
        return outColor;
    }

    void time_to_string(uint32_t time, bento::DynamicString& outputString)
    {
        time_t temp = time;
        tm* t = std::gmtime(&temp);
        std::stringstream ss; // or if you're going to print, just input directly into the output stream
        ss << std::put_time(t, "%Y-%m-%d %I:%M:%S %p");
        outputString = ss.str().c_str();
    }

    void duration_to_string(uint32_t duration, bento::DynamicString& outputString)
    {
        int hours = (duration / 3600) % 24;
        int minutes = (duration / 60) % 60;
        int seconds = (duration % 60);

        std::stringstream ss;
        if (hours != 0)
            ss << hours<<"h";
        if (minutes != 0)
            ss << minutes << "min";
        if (seconds != 0)
            ss << seconds << "s";
        outputString = ss.str().c_str();
    }
}
