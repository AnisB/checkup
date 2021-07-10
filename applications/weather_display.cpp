// SDK includes
#include <bento_memory/system_allocator.h>
#include <bento_base/log.h>
#include <bento_resources/asset_database.h>
#include <bento_tools/disk_serializer.h>
#include <checkup_rest/session.h>
#include <checkup_rest/weather_info.h>

// External includes
#include <string>

int main(int argc, char** argv)
{
	// Check that the id is there
	if (argc != 3)
		return -1;

    // Create an allocator for our application
    bento::SystemAllocator systemAllocator;

    // Create a session
    checkup::TSession session(systemAllocator);

    // Init the session
    session.init();

    // Do a ping request
    checkup::TRequest request(systemAllocator);
    request.api = "https://api.openweathermap.org/data/2.5/weather";
    request.content = "q=";
    request.content += argv[1];
    request.content += "&appid=";
    request.content += argv[2];
    session.execute(request);
    
    // Parse the weather info
    checkup::TWeatherInfo weatherInfo(systemAllocator);
    bool success = request.validity ? build_weather_data(request.result, weatherInfo) : false;

    // Log the answer
    bento::ILogger* logger = bento::default_logger();
    if (success)
    {
        logger->log(bento::LogLevel::info, "STATUS", "success");
        logger->log(bento::LogLevel::info, "Temperature", std::to_string(weatherInfo.temperature).c_str());
        logger->log(bento::LogLevel::info, "Min Temperature", std::to_string(weatherInfo.minTemperature).c_str());
        logger->log(bento::LogLevel::info, "Max Temperature", std::to_string(weatherInfo.maxTemperature).c_str());
    }
    else
    {
        logger->log(bento::LogLevel::info, "ERROR", "Request failed");
    }

    // Terminate the session
    session.terminate();

    // We are done!
    return 0;
}