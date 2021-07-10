// SDK includes
#include <bento_memory/system_allocator.h>
#include <bento_base/log.h>
#include <bento_resources/asset_database.h>
#include <bento_tools/disk_serializer.h>
#include <checkup_rest/session.h>
#include <checkup_rest/weather.h>

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
    for (int requestIdx = 0; requestIdx < requestCount; ++requestIdx)
    {
        // Do a weather request
        checkup::TRequest request(systemAllocator);
        request.api = "https://api.openweathermap.org/data/2.5/weather";
        request.content = "q=";
        request.content += argv[2 + requestIdx];
        request.content += "&appid=";
        request.content += argv[1];
        session.execute(request);

        // Parse the weather info
        checkup::TWeatherInfo weatherInfo(systemAllocator);
        build_weather_data(request.result, weatherInfo, logger);
    }

    // Terminate the session
    session.terminate();

    // We are done!
    return 0;
}