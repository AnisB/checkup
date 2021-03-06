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
    viewportLayout.general.start_y = (int)(viewport.startY * height + displayPadding);
    viewportLayout.general.size_x = (int)(viewport.width * width - displayPadding*2);
    viewportLayout.general.size_y = (int)(viewport.height * height * 0.25 - displayPadding * 2);

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
            imageView->setFixedWidth((int)(rect.size_y * scale));
            imageView->setFixedHeight((int)(rect.size_y * scale));
        }
    }
    else
    {
        auto imageIt = icons.find("images/Empty.png");
        auto imageView = layout.add<ImageView>(imageIt->second);
        imageView->setFixedWidth((int)(rect.size_y * scale));
        imageView->setFixedHeight((int)(rect.size_y * scale));
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
        imageView->setFixedWidth((int)(rect.size_y * scale));
        imageView->setFixedHeight((int)(rect.size_y * scale));
    }
}

ReadyToGoScreen::ReadyToGoScreen(SDL_Window* pwindow, int width, int height,
    const TDisplayData& displayData,
    bento::IAllocator& allocator)
    : Screen(pwindow, Vector2i(width, height), "Ready To Go")
    , _allocator(allocator)
    , m_displayData(displayData)
    , m_width(width)
    , m_height(height)
    , m_window(pwindow)
{
        // Load all images
    ListImages images = loadImageDirectory(SDL_GetRenderer(pwindow), "images");
    for (auto& icon : images) 
        m_icons[icon.path] = icon.tex;

    // Loop through the weather viewports
    uint32_t numWeatherViewports = displayData.weatherInfoArray.size();
    for(uint32_t viewportIdx = 0; viewportIdx < numWeatherViewports; ++viewportIdx)
    {
        weather_viewport(viewportIdx);
    }

    // Loop through the viewports
    uint32_t numRouteViewport = displayData.routeViewportArray.size();
    for (uint32_t viewportIdx = 0; viewportIdx < numRouteViewport; ++viewportIdx)
    {
        route_viewport(viewportIdx);
    }

    // Loop through the viewports
    uint32_t numRatpViewports = displayData.ratpViewportArray.size();
    for (uint32_t viewportIdx = 0; viewportIdx < numRatpViewports; ++viewportIdx)
    {
        ratp_viewport(viewportIdx);
    }

    performLayout(mSDL_Renderer);
}

ReadyToGoScreen::~ReadyToGoScreen()
{

}

void ReadyToGoScreen::weather_viewport(int viewportIdx)
{
    bento::DynamicString labelBuild(_allocator);

    // Grab the current weather data
    const checkup::TWeatherViewport& weatherViewport = m_displayData.weatherViewportArray[viewportIdx];
    const checkup::TWeatherInfo& weatherInfo = m_displayData.weatherInfoArray[viewportIdx];
    const checkup::TForecastInfo& forecastInfo = m_displayData.forecastInfoArray[viewportIdx];

    // Build the layout data
    TWeatherViewportLayout vl;
    build_weather_viewport_layout(weatherViewport, m_width, m_height, vl);

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
        generalWindow.setPosition(Vector2i{ vl.general.start_x, vl.general.start_y });
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

        display_hourly_weather_icon(layout, m_icons, weatherHours.morningIdx, 0.25, forecastInfo, vl.weatherD, isDay, _allocator);
        display_hourly_weather_icon(layout, m_icons, weatherHours.afternoonIdx, 0.25, forecastInfo, vl.weatherD, isDay, _allocator);
        display_hourly_weather_icon(layout, m_icons, weatherHours.eveningIdx, 0.25, forecastInfo, vl.weatherD, isDay, _allocator);
    }
    {
        auto& img_window = wdg<Window>("J + 1");
        img_window.setPosition(Vector2i{ vl.weatherD_1.start_x, vl.weatherD_1.start_y });
        img_window.setFixedWidth(vl.weatherD_1.size_x);
        img_window.setFixedHeight(vl.weatherD_1.size_y);
        img_window.setEnabled(false);
        auto& layout = img_window.withLayout<GroupLayout>();

        display_hourly_weather_icon(layout, m_icons, weatherHours.morningIdx_1, 0.25, forecastInfo, vl.weatherD_1, isDay, _allocator);
        display_hourly_weather_icon(layout, m_icons, weatherHours.afternoonIdx_1, 0.25, forecastInfo, vl.weatherD_1, isDay, _allocator);
        display_hourly_weather_icon(layout, m_icons, weatherHours.eveningIdx_1, 0.25, forecastInfo, vl.weatherD_1, isDay, _allocator);
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

        display_daily_weather_icon(layout, m_icons, weatherHours.jIdx_2, 0.25, forecastInfo, vl.weatherW, isDay, _allocator);
        display_daily_weather_icon(layout, m_icons, weatherHours.jIdx_3, 0.25, forecastInfo, vl.weatherW, isDay, _allocator);
        display_daily_weather_icon(layout, m_icons, weatherHours.jIdx_4, 0.25, forecastInfo, vl.weatherW, isDay, _allocator);
    }
    {
        auto& precipWindow = wdg<Window>("Precipitation Probability");
        precipWindow.setPosition(Vector2i{ vl.preci.start_x, vl.preci.start_y });
        precipWindow.setFixedWidth(vl.preci.size_x);
        precipWindow.setFixedHeight(vl.preci.size_y);
        precipWindow.setEnabled(false);
        auto& precipWindowLayout = precipWindow.withLayout<GroupLayout>();

        Graph* precipGraph = precipWindowLayout.add<Graph>("Probablity");
        precipGraph->setFixedHeight((int)(weatherViewport.height * m_height * 0.3f));
        precipGraph->setHeader("100%");
        precipGraph->setFooter("0%");
        precipGraph->setForegroundColor(debugColor);
        {
            std::vector<float>& func = precipGraph->values();

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

        auto& rainWindow = wdg<Window>("Rain Amount");
        rainWindow.setPosition(Vector2i{ vl.preci.start_x, vl.preci.start_y });
        rainWindow.setFixedWidth(vl.preci.size_x);
        rainWindow.setFixedHeight(vl.preci.size_y);
        rainWindow.setEnabled(false);
        auto& rainWindowLayout = rainWindow.withLayout<GroupLayout>();

        Graph* rainGraph = rainWindow.add<Graph>("Height");
        rainGraph->setFixedHeight((int)(weatherViewport.height * m_height * 0.3f));
        rainGraph->setHeader("10mm");
        rainGraph->setFooter("0mm");
        rainGraph->setForegroundColor(debugColor);
        {
            std::vector<float>& func = rainGraph->values();

            uint32_t numHours = forecastInfo.hourlyForecast.size();
            numHours = numHours < 24 ? numHours : 24;
            func.resize(numHours);
            for (uint32_t i = 0; i < numHours; ++i)
            {
                func[i] = forecastInfo.hourlyForecast[i].rain1h_o, 0.0f;
            }
        }


        ForecastWindows forecastWindows;
        forecastWindows.pop = &precipWindow;
        forecastWindows.rain = &rainWindow;
        m_forecastWindows.push_back(forecastWindows);
    }
}


void ReadyToGoScreen::route_viewport(int viewportIdx)
{
    bento::DynamicString labelBuild(_allocator);

    // Grab the current weather data
    const checkup::TRouteViewport& routeViewport = m_displayData.routeViewportArray[viewportIdx];
    const checkup::TRouteInfo& routeInfo = m_displayData.routeInfoArray[viewportIdx];
    {
        // Create the window
        auto& generalWindow = wdg<Window>(routeViewport.vehicle.c_str());

        int displayPadding = 3;
        int start_x = (int)(routeViewport.startX * m_width + displayPadding);
        int start_y = (int)(routeViewport.startY * m_height + displayPadding);
        int size_x = (int)(routeViewport.width * m_width - displayPadding * 2);
        int size_y = (int)(routeViewport.height * m_height - displayPadding * 2);

        // Set it's size and position
        generalWindow.setPosition(Vector2i{start_x, start_y });
        generalWindow.setFixedWidth(size_x);
        generalWindow.setFixedHeight(size_y);
        generalWindow.setEnabled(false);

        bento::DynamicString str(_allocator);
        checkup::duration_to_string(routeInfo.duration, str);

        labelBuild = routeViewport.name.c_str();
        labelBuild += " ";
        labelBuild += to_string_with_precision<float>(routeInfo.length, 1).c_str();
        labelBuild += " km ";
        labelBuild += str.c_str();

        auto& generalLayout = generalWindow.withLayout<GroupLayout>();
        generalLayout.label(labelBuild.c_str(), "Raleway-Regular", 13);
    }
}

int min_value(int a, int b)
{
    return a < b ? a : b;
}

std::vector<std::string> make_string_multiline(const std::string& inputString, int targetCharPerLine)
{
    // Output structure
    std::vector<std::string> outputString;
    int inputLength = (int)inputString.length();
    int currentChar = 0;
    while (currentChar < (inputLength - 1))
    {
        int lineEndChar = min_value(currentChar + targetCharPerLine, inputLength - 1);
        while ((inputString[lineEndChar - 1] != ' ') && (lineEndChar != (inputLength - 1)))
            lineEndChar--;
        const std::string& lineString = inputString.substr(currentChar, lineEndChar - currentChar);
        outputString.push_back(lineString);
        currentChar = lineEndChar;
    }
    return outputString;
}


void ReadyToGoScreen::ratp_viewport(int viewportIdx)
{
    bento::DynamicString labelBuild(_allocator);

    // Grab the current weather data
    const checkup::TRATPViewport& ratpViewport = m_displayData.ratpViewportArray[viewportIdx];
    const checkup::TRATPInfo& ratpInfo = m_displayData.ratpInfo;
    {
        // Create the window
        auto& generalWindow = wdg<Window>(ratpViewport.name.c_str());

        int displayPadding = 3;
        int start_x = (int)(ratpViewport.startX * m_width + displayPadding);
        int start_y = (int)(ratpViewport.startY * m_height + displayPadding);
        int size_x = (int)(ratpViewport.width * m_width - displayPadding * 2);
        int size_y = (int)(ratpViewport.height * m_height - displayPadding * 2);

        // Set it's size and position
        generalWindow.setPosition(Vector2i{ start_x, start_y });
        generalWindow.setFixedWidth(size_x);
        generalWindow.setFixedHeight(size_y);
        generalWindow.setEnabled(false);

        const auto& lineName = ratpViewport.line;
        auto& generalLayout = generalWindow.withLayout<GroupLayout>();
        auto it = ratpInfo.linesInfo.find(lineName.c_str());
        const auto& multLineString = make_string_multiline(it->second.message, 55);
        bento::Vector3 color = checkup::state_to_color(it->second.state);
        for each (auto line in multLineString)
            generalLayout.label(line.c_str(), "Raleway-Regular", 11).setColor(sdlgui::Color(color.x, color.y, color.z, 1.0));
    }
}


void ReadyToGoScreen::draw(SDL_Renderer* renderer)
{
    std::time_t time = std::time(nullptr);

    bento::DynamicString labelCaption(_allocator);
    uint32_t weatherViewports = (uint32_t)m_timeLabels.size();
    for (uint32_t labelIdx = 0; labelIdx < weatherViewports; ++labelIdx)
    {
        auto& lbl = m_timeLabels[labelIdx];
        checkup::time_to_string(time + lbl.timeOffset, labelCaption);
        lbl.label->setCaption(labelCaption.c_str());
    }

    // Initial update time
    uint32_t sdlTicks = SDL_GetTicks();

    // We switch which graphs are displayed every 10 seconds
    int status = sdlTicks / (1000 * 10) % 2;
    for (uint32_t graphIdx = 0; graphIdx < weatherViewports; ++graphIdx)
    {
        auto& forecastWindows = m_forecastWindows[graphIdx];
        if (status == 1)
        {
            forecastWindows.pop->setVisible(true);
            forecastWindows.pop->setEnabled(true);
            forecastWindows.rain->setVisible(false);
            forecastWindows.rain->setEnabled(false);
        }
        else
        {
            forecastWindows.pop->setVisible(false);
            forecastWindows.pop->setEnabled(false);
            forecastWindows.rain->setVisible(true);
            forecastWindows.rain->setEnabled(true);
        }
    }

    Screen::draw(renderer);
}
bool ReadyToGoScreen::keyboardEvent(int key, int scancode, int action, int modifiers)
{
    if (Screen::keyboardEvent(key, scancode, action, modifiers))
        return true;
    return false;
}
