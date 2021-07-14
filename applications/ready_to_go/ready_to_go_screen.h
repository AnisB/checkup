// SDL GUI includes
#include <sdlgui/screen.h>
#include <sdlgui/window.h>

// SDK includes
#include <checkup_weather/weather.h>
#include <checkup_weather/forecast.h>
#include <checkup_route/route.h>

struct TDisplayData
{
    ALLOCATOR_BASED;
    TDisplayData(bento::IAllocator& allocator)
        : weatherViewportArray(allocator)
        , weatherInfoArray(allocator)
        , forecastInfoArray(allocator)
        , routeViewportArray(allocator)
        , routeInfoArray(allocator)
        , owmToken(allocator)
        , hereToken(allocator)
    {

    }
    bento::DynamicString owmToken;
    bento::DynamicString hereToken;
    bento::Vector<checkup::TWeatherViewport> weatherViewportArray;
    bento::Vector<checkup::TWeatherInfo> weatherInfoArray;
    bento::Vector<checkup::TForecastInfo> forecastInfoArray;
    bento::Vector<checkup::TRouteViewport> routeViewportArray;
    bento::Vector<checkup::TRouteInfo> routeInfoArray;
};

struct TimeLabel
{
    uint32_t timeOffset;
    sdlgui::Label* label;
};

class ReadyToGoScreen : public sdlgui::Screen
{
private:
    std::map<std::string, SDL_Texture*> m_icons;
    std::vector<TimeLabel> m_timeLabels;
    const TDisplayData& m_displayData;
    int m_width;
    int m_height;
    SDL_Window* m_window;
    bento::IAllocator& _allocator;

public:
    ReadyToGoScreen(SDL_Window* pwindow, int width, int height,
        const TDisplayData& displayData,
        bento::IAllocator& allocator);
    ~ReadyToGoScreen();

    void weather_viewport(int viewportIdx);
    void route_viewport(int viewportIdx);
    virtual void draw(SDL_Renderer* renderer);
    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers);
};