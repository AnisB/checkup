// SDK includes
#include <bento_memory/system_allocator.h>
#include <bento_base/log.h>
#include <bento_resources/asset_database.h>
#include <bento_tools/disk_serializer.h>
#include <checkup_rest/session.h>
#include <checkup_weather/weather.h>
#include <checkup_weather/forecast.h>

// External includes
#include <string>

int main(int argc, char** argv)
{
	// Check that the id is there
	if (argc < 3)
		return -1;

    // Create an allocator for our application
    bento::SystemAllocator systemAllocator;

    // Create a session
    checkup::TSession session(systemAllocator);

    // Init the session
    session.init();

    // Grab the logger
    bento::ILogger* logger = bento::default_logger();

    // Number of requests that need to be processed
    int requestCount = argc - 2;
    checkup::TRequest request(systemAllocator);
    for (int requestIdx = 0; requestIdx < requestCount; ++requestIdx)
    {
        // Build and execute the weather request
        build_weather_request(argv[2 + requestIdx], argv[1], request);
        session.execute(request);
        checkup::TWeatherInfo weatherInfo(systemAllocator);
        build_weather_data(request.result, weatherInfo, logger);

        // Build and execute the forecast request
        build_forecast_request(weatherInfo.latitude, weatherInfo.longitude, argv[1], request);
        session.execute(request);
        checkup::TForecastInfo forecastInfo(systemAllocator);
        build_forecast_data(request.result, forecastInfo, logger);
    }

    // Terminate the session
    session.terminate();

    // We are done!
    return 0;
}