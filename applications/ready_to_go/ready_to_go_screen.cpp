// External includes
#include <fstream>
#include <string>
#include <json.hpp>

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

#include "ready_to_go_screen.h"

using namespace sdlgui;

template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}


struct TWindowRect
{
    int32_t start_x;
    int32_t start_y;
    int32_t size_x;
    int32_t size_y;
};

struct TWeatherViewportLayout
{
    TWindowRect general;
    TWindowRect temp;
    TWindowRect preci;
    TWindowRect weatherD;
    TWindowRect weatherD_1;
    TWindowRect weatherW;
};

void build_weather_viewport_layout(const checkup::TWeatherViewport& viewport, int width, int height, TWeatherViewportLayout& viewportLayout)
{
    int displayPadding = 3;
    viewportLayout.general.start_x = (int)(viewport.startX * width + displayPadding);
    viewportLayout.general.start_y = (int)(viewport.startY * height);
    viewportLayout.general.size_x = (int)(viewport.width * width - displayPadding*2);
    viewportLayout.general.size_y = (int)(viewport.height * height * 0.25);

    viewportLayout.temp.start_x = (int)(viewport.startX * width + displayPadding);
    viewportLayout.temp.start_y = (int)((viewport.startY + 0.25 * viewport.height) * height + displayPadding);
    viewportLayout.temp.size_x = (int)(viewport.width * width * 0.5 - displayPadding * 2);
    viewportLayout.temp.size_y = (int)(viewport.height * height * 0.25 - displayPadding * 2);

    viewportLayout.preci.start_x = (int)((viewport.startX) * width + displayPadding);
    viewportLayout.preci.start_y = (int)((viewport.startY + 0.5 * viewport.height) * height + displayPadding);
    viewportLayout.preci.size_x = (int)(viewport.width * width * 0.5 - displayPadding * 2);
    viewportLayout.preci.size_y = (int)(viewport.height * height * 0.5 - displayPadding * 2);

    viewportLayout.weatherD.start_x = (int)((viewport.startX + viewport.width * 0.5) * width + displayPadding);
    viewportLayout.weatherD.start_y = (int)((viewport.startY + 0.25 * viewport.height) * height + displayPadding);
    viewportLayout.weatherD.size_x = (int)(viewport.width * width * 0.1666 - displayPadding * 2);
    viewportLayout.weatherD.size_y = (int)(viewport.height * height * 0.75 - displayPadding * 2);

    viewportLayout.weatherD_1.start_x = (int)((viewport.startX + viewport.width * 0.666) * width + displayPadding);
    viewportLayout.weatherD_1.start_y = (int)((viewport.startY + 0.25 * viewport.height) * height + displayPadding);
    viewportLayout.weatherD_1.size_x = (int)(viewport.width * width * 0.1666 - displayPadding * 2);
    viewportLayout.weatherD_1.size_y = (int)(viewport.height * height * 0.75 - displayPadding * 2);

    viewportLayout.weatherW.start_x = (int)((viewport.startX + viewport.width * 0.8333) * width + displayPadding);
    viewportLayout.weatherW.start_y = (int)((viewport.startY + 0.25 * viewport.height) * height + displayPadding);
    viewportLayout.weatherW.size_x = (int)(viewport.width * width * 0.16666 - displayPadding * 2);
    viewportLayout.weatherW.size_y = (int)(viewport.height * height * 0.75 - displayPadding * 2);
}

struct TWeatherHours
{
    int32_t morningIdx;
    int32_t afternoonIdx;
    int32_t eveningIdx;

    int32_t morningIdx_1;
    int32_t afternoonIdx_1;
    int32_t eveningIdx_1;

    int32_t jIdx_2;
    int32_t jIdx_3;
    int32_t jIdx_4;
};

void build_weather_hours(const checkup::TForecastInfo& forecastInfo, TWeatherHours& weatherHours)
{
    // Convert the current time to the explicit time
    time_t currentTime = forecastInfo.currentForecast.time;
    tm* currentTM = std::gmtime(&currentTime);
    int currentHour = currentTM->tm_hour;

    // Initialize all the indices
    weatherHours.morningIdx = INT32_MAX;
    weatherHours.afternoonIdx = INT32_MAX;
    weatherHours.eveningIdx = INT32_MAX;

    weatherHours.morningIdx_1 = INT32_MAX;
    weatherHours.afternoonIdx_1 = INT32_MAX;
    weatherHours.eveningIdx_1 = INT32_MAX;

    weatherHours.jIdx_2 = INT32_MAX;
    weatherHours.jIdx_3 = INT32_MAX;
    weatherHours.jIdx_4 = INT32_MAX;

    // Is there any morning hour available?
    if (currentHour <= 12)
    {
        if (currentHour <= 8)
        {
            weatherHours.morningIdx = 8 - currentHour;
        }
        else
        {
            weatherHours.morningIdx = currentHour;
        }
    }

    // Is there any afternoon hour available?
    if (currentHour <= 18)
    {
        if (currentHour <= 15)
        {
            weatherHours.afternoonIdx = 15 - currentHour;
        }
        else
        {
            weatherHours.afternoonIdx = currentHour;
        }
    }

    if (currentHour <= 21)
    {
        weatherHours.eveningIdx = 21 - currentHour;
    }
    else
    {
        weatherHours.eveningIdx = currentHour;
    }

    weatherHours.morningIdx_1 = 24 - currentHour + 8;
    weatherHours.afternoonIdx_1 = 24 - currentHour + 15;
    weatherHours.eveningIdx_1 = 24 - currentHour + 21;

    weatherHours.jIdx_2 = 2;
    weatherHours.jIdx_3 = 3;
    weatherHours.jIdx_4 = 4;
}

void evaluate_icon_for_weather(checkup::TWeatherCategory category, bool isDay, bento::DynamicString& label)
{
    label = "images/";
    label += category_to_string(category);
    label += isDay ? "_day.png" : "_night.png";
}

void display_hourly_weather_icon(Widget& layout, const std::map<std::string, SDL_Texture*>& icons,
    int hourIdx, float scale,
    const checkup::TForecastInfo& forecastInfo,
    const TWindowRect& rect,
    bool isDay, bento::IAllocator& allocator)
{
    if (hourIdx != INT32_MAX)
    {
        bento::DynamicString labelBuild(allocator);
        evaluate_icon_for_weather(forecastInfo.hourlyForecast[hourIdx].weatherCategory, isDay, labelBuild);
        auto imageIt = icons.find(labelBuild.c_str());
        if (imageIt != icons.end())
        {
            auto imageView = layout.add<ImageView>(imageIt->second);
            imageView->setFixedWidth(rect.size_y * scale);
            imageView->setFixedHeight(rect.size_y * scale);
        }
    }
    else
    {
        auto imageIt = icons.find("images/Empty.png");
        auto imageView = layout.add<ImageView>(imageIt->second);
        imageView->setFixedWidth(rect.size_y * scale);
        imageView->setFixedHeight(rect.size_y * scale);
    }
}

void display_daily_weather_icon(Widget& layout, const std::map<std::string, SDL_Texture*>& icons,
    int dayIdx, float scale,
    const checkup::TForecastInfo& forecastInfo,
    const TWindowRect& rect,
    bool isDay, bento::IAllocator& allocator)
{
    bento::DynamicString labelBuild(allocator);
    evaluate_icon_for_weather(forecastInfo.dailyForecast[dayIdx].weatherCategory, isDay, labelBuild);
    auto imageIt = icons.find(labelBuild.c_str());
    if (imageIt != icons.end())
    {
        auto imageView = layout.add<ImageView>(imageIt->second);
        imageView->setFixedWidth(rect.size_y * scale);
        imageView->setFixedHeight(rect.size_y * scale);
    }
}

ReadyToGoScreen::ReadyToGoScreen(SDL_Window* pwindow, int width, int height,
    const bento::Vector<checkup::TWeatherViewport>& weatherViewportArray,
    const bento::Vector<checkup::TWeatherInfo>& weatherInfoArray,
    const bento::Vector<checkup::TForecastInfo>& forecastInfoArray,
    bento::IAllocator& allocator)
    : Screen(pwindow, Vector2i(width, height), "Ready To Go")
    , _allocator(allocator)
    , forecastInfoArray(forecastInfoArray)
{
    bento::DynamicString labelBuild(allocator);

    // Load all images
    ListImages images = loadImageDirectory(SDL_GetRenderer(pwindow), "images");
    for (auto& icon : images) 
        m_icons[icon.path] = icon.tex;

    // Loop through the viewports
    uint32_t numWeatherViewports = weatherInfoArray.size();
    for(uint32_t viewportIdx = 0; viewportIdx < numWeatherViewports; ++viewportIdx)
    {
        // Grab the current weather data
        const checkup::TWeatherViewport& weatherViewport = weatherViewportArray[viewportIdx];
        const checkup::TWeatherInfo& weatherInfo = weatherInfoArray[viewportIdx];
        const checkup::TForecastInfo& forecastInfo = forecastInfoArray[viewportIdx];

        // Build the layout data
        TWeatherViewportLayout vl;
        build_weather_viewport_layout(weatherViewport, width, height, vl);

        // Build the weather 
        TWeatherHours weatherHours;
        build_weather_hours(forecastInfo, weatherHours);

        // Evaluate if we are during the day or not
        bool isDay = checkup::is_day(forecastInfo.currentForecast.time, forecastInfo.currentForecast.sunrise, forecastInfo.currentForecast.sunset);
        Color debugColor = Color(weatherViewport.debugColor.x, weatherViewport.debugColor.y, weatherViewport.debugColor.z, 1.0);
        {
            // Create the window
            auto& generalWindow = wdg<Window>("General");

            // Set it's size and position
            generalWindow.setPosition(Vector2i{vl.general.start_x, vl.general.start_y });
            generalWindow.setFixedWidth(vl.general.size_x);
            generalWindow.setFixedHeight(vl.general.size_y);
            generalWindow.setEnabled(false);

            checkup::time_to_string(forecastInfo.currentForecast.time, labelBuild);
            auto& generalLayout = generalWindow.withLayout<GroupLayout>();
            auto& tempLabel = generalLayout.label("", "Raleway-Regular", 24);
            auto& boxLayout = tempLabel._and().widget().boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 32);
            boxLayout.label(weatherInfo.cityName.c_str(), "Raleway-Regular", 32).setColor(debugColor);
            Label& currentLabel = boxLayout.label(labelBuild.c_str(), "Raleway-Regular", 32);
            currentLabel.withId("time-label");
            currentLabel.setColor(debugColor);
            m_timeLabels.push_back({ forecastInfo.timeOffset, &currentLabel });
        }
        {
            // Create the window
            auto& tWindow = wdg<Window>("Temperature");

            // Set it's size and position
            tWindow.setPosition(Vector2i{ vl.temp.start_x, vl.temp.start_y });
            tWindow.setFixedWidth(vl.temp.size_x);
            tWindow.setFixedHeight(vl.temp.size_y);
            tWindow.setEnabled(false);

            auto& temperatureWindow = tWindow.withLayout<GroupLayout>();
            // Parent label of the temporatures
            auto& tempLabel = temperatureWindow.label("", "Raleway-Regular", 12);
            bento::Vector3 color;
            {
                auto& boxLayout1 = tempLabel._and().widget().boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 16);
                // Current temperature
                color = checkup::temperature_to_color(weatherInfo.temperature);
                boxLayout1.label("Current:", "Raleway-Regular", 18).setColor(Color(1.0f, 1.0, 1.0, 1.0));
                boxLayout1.label(to_string_with_precision<float>(weatherInfo.temperature, 2).c_str(), "Raleway-Regular", 18).setColor(Color(color.x, color.y, color.z, 1.0));

                // Felt temperature
                color = checkup::temperature_to_color(weatherInfo.feltTemperature);
                boxLayout1.label("Felt:", "Raleway-Regular", 18).setColor(Color(1.0f, 1.0, 1.0, 1.0));
                boxLayout1.label(to_string_with_precision<float>(weatherInfo.feltTemperature, 2).c_str(), "Raleway-Regular", 18).setColor(Color(color.x, color.y, color.z, 1.0));
            }

            {
                auto& boxLayout2 = tempLabel._and().widget().boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 16);

                // Max temperature
                color = checkup::temperature_to_color(weatherInfo.maxTemperature);
                boxLayout2.label("Max:", "Raleway-Regular", 18).setColor(Color(1.0f, 1.0, 1.0, 1.0));
                boxLayout2.label(to_string_with_precision<float>(weatherInfo.maxTemperature, 2).c_str(), "Raleway-Regular", 18).setColor(Color(color.x, color.y, color.z, 1.0));

                // Min temperature
                color = checkup::temperature_to_color(weatherInfo.minTemperature);
                boxLayout2.label("Min:", "Raleway-Regular", 18).setColor(Color(1.0f, 1.0, 1.0, 1.0));
                boxLayout2.label(to_string_with_precision<float>(weatherInfo.minTemperature, 2).c_str(), "Raleway-Regular", 18).setColor(Color(color.x, color.y, color.z, 1.0));
            }
        }

        {
            auto& img_window = wdg<Window>("J");
            img_window.setPosition(Vector2i{ vl.weatherD.start_x, vl.weatherD.start_y });
            img_window.setFixedWidth(vl.weatherD.size_x);
            img_window.setFixedHeight(vl.weatherD.size_y);
            img_window.setEnabled(false);
            auto& layout = img_window.withLayout<GroupLayout>();

            display_hourly_weather_icon(layout, m_icons, weatherHours.morningIdx, 0.25, forecastInfo, vl.weatherD, isDay, allocator);
            display_hourly_weather_icon(layout, m_icons, weatherHours.afternoonIdx, 0.25, forecastInfo, vl.weatherD, isDay, allocator);
            display_hourly_weather_icon(layout, m_icons, weatherHours.eveningIdx, 0.25, forecastInfo, vl.weatherD, isDay, allocator);
        }
        {
            auto& img_window = wdg<Window>("J + 1");
            img_window.setPosition(Vector2i{ vl.weatherD_1.start_x, vl.weatherD_1.start_y });
            img_window.setFixedWidth(vl.weatherD_1.size_x);
            img_window.setFixedHeight(vl.weatherD_1.size_y);
            img_window.setEnabled(false);
            auto& layout = img_window.withLayout<GroupLayout>();

            display_hourly_weather_icon(layout, m_icons, weatherHours.morningIdx_1, 0.25, forecastInfo, vl.weatherD_1, isDay, allocator);
            display_hourly_weather_icon(layout, m_icons, weatherHours.afternoonIdx_1, 0.25, forecastInfo, vl.weatherD_1, isDay, allocator);
            display_hourly_weather_icon(layout, m_icons, weatherHours.eveningIdx_1, 0.25, forecastInfo, vl.weatherD_1, isDay, allocator);
        }

        {
            auto& img_window = wdg<Window>("W");
            img_window.setPosition(Vector2i{ vl.weatherW.start_x, vl.weatherW.start_y });
            img_window.setFixedWidth(vl.weatherW.size_x);
            img_window.setFixedHeight(vl.weatherW.size_y);
            img_window.setEnabled(false);
            auto& layout = img_window.withLayout<GroupLayout>();
            layout.setFixedWidth(vl.weatherW.size_x);
            layout.setFixedHeight(vl.weatherW.size_y);

            display_daily_weather_icon(layout, m_icons, weatherHours.jIdx_2, 0.25, forecastInfo, vl.weatherW, isDay, allocator);
            display_daily_weather_icon(layout, m_icons, weatherHours.jIdx_3, 0.25, forecastInfo, vl.weatherW, isDay, allocator);
            display_daily_weather_icon(layout, m_icons, weatherHours.jIdx_4, 0.25, forecastInfo, vl.weatherW, isDay, allocator);
        }
        {
            auto& rainWindow = wdg<Window>("Precipitation");
            rainWindow.setPosition(Vector2i{ vl.preci.start_x, vl.preci.start_y });
            rainWindow.setFixedWidth(vl.preci.size_x);
            rainWindow.setFixedHeight(vl.preci.size_y);
            rainWindow.setEnabled(false);
            auto& rainWindowLayout = rainWindow.withLayout<GroupLayout>();

            Graph* graph = rainWindowLayout.add<Graph>("Rain Curves");
            graph->setFixedHeight((int)(weatherViewport.height * height * 0.3f));
            graph->setHeader("100%");
            graph->setFooter("0%");
            graph->setForegroundColor(debugColor);
            std::vector<float>& func = graph->values();

            uint32_t numHours = forecastInfo.hourlyForecast.size();
            numHours = numHours < 24 ? numHours : 24;
            func.resize(numHours);
            for (uint32_t i = 0; i < numHours; ++i)
            {
                func[i] = forecastInfo.hourlyForecast[i].pop, 0.02f;
                if (func[i] < 0.02f)
                    func[i] = 0.02f;
            }
        }
    }

    performLayout(mSDL_Renderer);
}

ReadyToGoScreen::~ReadyToGoScreen()
{

}

void ReadyToGoScreen::draw(SDL_Renderer* renderer)
{
    std::time_t time = std::time(nullptr);

    bento::DynamicString labelCaption(_allocator);
    uint32_t labelCount = m_timeLabels.size();
    for (uint32_t labelIdx = 0; labelIdx < labelCount; ++labelIdx)
    {
        auto& lbl = m_timeLabels[labelIdx];
        checkup::time_to_string(time + lbl.timeOffset, labelCaption);
        lbl.label->setCaption(labelCaption.c_str());
    }

    Screen::draw(renderer);
}
bool ReadyToGoScreen::keyboardEvent(int key, int scancode, int action, int modifiers)
{
    if (Screen::keyboardEvent(key, scancode, action, modifiers))
        return true;
    return false;
}
