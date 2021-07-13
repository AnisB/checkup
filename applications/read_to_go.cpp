// External includes
#include <fstream>
#include <string>
#include <json.hpp>
#include <sdlgui/screen.h>
#include <sdlgui/window.h>
#include <sdlgui/layout.h>
#include <sdlgui/label.h>
#include <sdlgui/checkbox.h>
#include <sdlgui/button.h>
#include <sdlgui/toolbutton.h>
#include <sdlgui/popupbutton.h>
#include <sdlgui/entypo.h>
#include <sdlgui/messagedialog.h>
#include <sdlgui/textbox.h>
#include <sdlgui/imagepanel.h>
#include <sdlgui/imageview.h>
#include <sdlgui/graph.h>
#include <memory>

// SDK includes
#include <bento_base/log.h>
#include <bento_base/platform.h>
#include <bento_memory/system_allocator.h>
#include <bento_resources/asset_database.h>
#include <bento_tools/disk_serializer.h>
#include <checkup_rest/session.h>
#include <checkup_weather/utility.h>
#include <checkup_weather/weather.h>
#include <checkup_weather/forecast.h>

#if defined(_WIN32)
#include <windows.h>
#endif
#include <iostream>

#if defined(_WIN32)
#include <SDL.h>
#include <SDL_ttf.h>
#else
#include <SDL2/SDL.h>
#include <SD2/SDL_ttf.h>
#endif
#if defined(_WIN32)
#include <SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

#undef main

using namespace sdlgui;

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

class ReadyToGoScreen : public Screen
{
public:
    ReadyToGoScreen(SDL_Window* pwindow, int width, int height,
        const bento::Vector<checkup::TWeatherViewport>& weatherViewportArray,
        const bento::Vector<checkup::TWeatherInfo>& weatherInfoArray,
        const bento::Vector<checkup::TForecastInfo>& forecastInfoArray,
        bento::IAllocator& allocator)
    : Screen(pwindow, Vector2i(width, height), "Ready To Go")
    {
        // Loop through the viewports
        uint32_t numWeatherViewports = weatherInfoArray.size();
        for(uint32_t viewportIdx = 0; viewportIdx < numWeatherViewports; ++viewportIdx)
        {
            // Grab the current weather data
            const checkup::TWeatherViewport& weatherViewport = weatherViewportArray[viewportIdx];
            const checkup::TWeatherInfo& weatherInfo = weatherInfoArray[viewportIdx];
            const checkup::TForecastInfo& forecastInfo = forecastInfoArray[viewportIdx];

            // Create the window
            auto& guiWindow = window("Temperature");
            auto& temperatureWindow = guiWindow.withLayout<GroupLayout>();
            
            // Set it's size and position
            temperatureWindow.setPosition(Vector2i{(int)((weatherViewport.startX) * width), (int)((weatherViewport.startY + weatherViewport.height * 0.2) * height) });
            temperatureWindow.setFixedWidth(weatherViewport.width * width * 0.5);
            temperatureWindow.setFixedHeight((weatherViewport.height-0.2) * height * 0.5);
            temperatureWindow.setEnabled(false);

            // Parent label of the temporatures
            auto& tempLabel = temperatureWindow.label("", "Raleway-Regular", 24);
            bento::Vector3 color;
            {
                auto& boxLayout1 = tempLabel._and().widget().boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 24);
                // Current temperature
                color = checkup::temperature_to_color(weatherInfo.temperature);
                boxLayout1.label("Current:", "Raleway-Regular", 22).setColor(Color(1.0f, 1.0, 1.0, 1.0));
                boxLayout1.label(to_string_with_precision<float>(weatherInfo.temperature, 2).c_str(), "Raleway-Regular", 22).setColor(Color(color.x, color.y, color.z, 1.0));
             
                // Felt temperature
                color = checkup::temperature_to_color(weatherInfo.feltTemperature);
                boxLayout1.label("Felt:", "Raleway-Regular", 22).setColor(Color(1.0f, 1.0, 1.0, 1.0));
                boxLayout1.label(to_string_with_precision<float>(weatherInfo.feltTemperature, 2).c_str(), "Raleway-Regular", 22).setColor(Color(color.x, color.y, color.z, 1.0));
            }

            {
                auto& boxLayout2 = tempLabel._and().widget().boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 24);
                
                // Max temperature
                color = checkup::temperature_to_color(weatherInfo.maxTemperature);
                boxLayout2.label("Max:", "Raleway-Regular", 22).setColor(Color(1.0f, 1.0, 1.0, 1.0));
                boxLayout2.label(to_string_with_precision<float>(weatherInfo.maxTemperature, 2).c_str(), "Raleway-Regular", 22).setColor(Color(color.x, color.y, color.z, 1.0));

                // Min temperature
                color = checkup::temperature_to_color(weatherInfo.minTemperature);
                boxLayout2.label("Min:", "Raleway-Regular", 22).setColor(Color(1.0f, 1.0, 1.0, 1.0));
                boxLayout2.label(to_string_with_precision<float>(weatherInfo.minTemperature, 2).c_str(), "Raleway-Regular", 22).setColor(Color(color.x, color.y, color.z, 1.0));
            }

            {
                auto& rainWindow = wdg<Window>("Precipitation");
                rainWindow.setPosition(Vector2i{ (int)(weatherViewport.startX * width), (int)((weatherViewport.height * 0.5 + weatherViewport.startY) * height)});
                rainWindow.setFixedWidth(weatherViewport.width * width);
                rainWindow.setFixedHeight(weatherViewport.height * height * 0.5);
                rainWindow.setEnabled(false);
                auto& rainWindowLayout = rainWindow.withLayout<GroupLayout>();

                Graph* graph = rainWindowLayout.add<Graph>("Rain Curves");
                graph->setFixedHeight(weatherViewport.height * height * 0.3);
                graph->setHeader("100%");
                graph->setFooter("0%");
                graph->setForegroundColor(Color(weatherViewport.debugColor.x, weatherViewport.debugColor.y, weatherViewport.debugColor.z, 1.0));
                std::vector<float>& func = graph->values();
                func.resize(48);
                for (int i = 0; i < 48; ++i)
                    func[i] = max(forecastInfo.hourlyForecast[i].pop, 0.02);
            }
        }

        performLayout(mSDL_Renderer);
    }

    ~ReadyToGoScreen() { }

    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers)
    {
        if (Screen::keyboardEvent(key, scancode, action, modifiers))
            return true;
        return false;
    }

    virtual void draw(SDL_Renderer* renderer)
    {
        Screen::draw(renderer);
    }
};

int main()
{
    // Create an allocator for our application
    bento::SystemAllocator systemAllocator;

    // Create a session
    checkup::TSession session(systemAllocator);

    // Init the session
    session.init();

    // Grab the logger
    bento::ILogger* logger = bento::default_logger();
    
    // Read the config file to a string
    std::ifstream istream("config.json");
    std::string str((std::istreambuf_iterator<char>(istream)), std::istreambuf_iterator<char>());

    // Parse the config
    nlohmann::json jsonConfig = nlohmann::json::parse(str.c_str());

    // Grab the token
    const std::string& owmToken = jsonConfig["tokens"]["open_weather_map"].get<std::string>();

    // Grab the viewports
    auto viewportArrayJson = jsonConfig["viewports"];

    // Number of requests that need to be processed
    uint32_t viewportCount = (uint32_t)viewportArrayJson.size();

    // The set of weather viewpots
    bento::Vector<checkup::TWeatherViewport> weatherViewportArray(systemAllocator);

    // Loop through the viewports
    for (uint32_t viewportIdx = 0; viewportIdx < viewportCount; ++viewportIdx)
    {
        // Grab the current viewport
        auto viewportJson = viewportArrayJson[viewportIdx];
        if (viewportJson["type"].get<std::string>() != "Weather")
            continue;
        
        // Create new viewport
        checkup::TWeatherViewport& newWeatherViewport = weatherViewportArray.extend();
        newWeatherViewport.cityName = viewportJson["name"].get<std::string>().c_str();
        newWeatherViewport.startX = viewportJson["start_x"].get<float>();
        newWeatherViewport.startY = viewportJson["start_y"].get<float>();
        newWeatherViewport.width = viewportJson["size_x"].get<float>();
        newWeatherViewport.height = viewportJson["size_y"].get<float>();
        newWeatherViewport.debugColor.x = viewportJson["debug_color"]["r"].get<float>();
        newWeatherViewport.debugColor.y = viewportJson["debug_color"]["g"].get<float>();
        newWeatherViewport.debugColor.z = viewportJson["debug_color"]["b"].get<float>();
    }

    // Number of viewports to handle
    uint32_t numWeatherViewports = weatherViewportArray.size();

    // Allocate the structures that will be updates
    checkup::TRequest request(systemAllocator);
    bento::Vector<checkup::TWeatherInfo> weatherInfoArray(systemAllocator, numWeatherViewports);
    bento::Vector<checkup::TForecastInfo> forecastInfoArray(systemAllocator, numWeatherViewports);

    // Loop throught the viewpots
    for (uint32_t weatherIdx = 0; weatherIdx < numWeatherViewports; ++weatherIdx)
    {
        // Grab the current viewport
        const checkup::TWeatherViewport& currentViewport = weatherViewportArray[weatherIdx];
        
        // Build and execute the weather request
        build_weather_request(currentViewport.cityName.c_str(), owmToken.c_str(), request);
        session.execute(request);

        // build the weather data
        auto& curentWeatherInfo = weatherInfoArray[weatherIdx];
        build_weather_data(request.result, weatherInfoArray[weatherIdx]);

        // Build and execute the forecast request
        build_forecast_request(curentWeatherInfo.latitude, curentWeatherInfo.longitude, owmToken.c_str(), request);
        session.execute(request);

        // Build the forecast data
        auto& curentForecastInfo = forecastInfoArray[weatherIdx];
        build_forecast_data(request.result, curentForecastInfo);
    }

    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        bento::default_logger()->log(bento::LogLevel::error, "SDL Init", SDL_GetError());

    // ENable the attributes that we need
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Create window
    SDL_Window* window = SDL_CreateWindow("Ready To Go", 0, 0, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    // Check that the window was successfully made
    if(window == nullptr)
        bento::default_logger()->log(bento::LogLevel::error, "SDL Window", SDL_GetError());

    // Set the window full screen
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    // Create a gl context
    auto context = SDL_GL_CreateContext(window);

    // Create the renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Define the blend mode
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_ShowCursor(0);
    SDL_CaptureMouse(SDL_FALSE);

    // Build the UI
    ReadyToGoScreen programUI(window, 1280, 720, weatherViewportArray, weatherInfoArray, forecastInfoArray, systemAllocator);

    bool quit = false;
    SDL_Event e;
    while(!quit)
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            // User requests quit
            if( e.type == SDL_QUIT )
                quit = true;

            // We intentionally ignore mouse motions
            if (e.type == SDL_MOUSEMOTION)
                continue;

            // Inject the events to the UI
            programUI.onEvent(e);
        }
        
        // Clear the screen black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw the UI
        programUI.drawAll();

        // Render the rect to the screen
        SDL_RenderPresent(renderer);

        // Sleep 300 miliseconds
        SLEEP_FUNCTION(300);
    }

    // Terminate the session
    session.terminate();
    
    return 0;
}
