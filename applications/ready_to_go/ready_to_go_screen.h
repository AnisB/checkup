// SDL GUI includes
#include <sdlgui/screen.h>
#include <sdlgui/window.h>

// SDK includes
#include <checkup_weather/weather.h>
#include <checkup_weather/forecast.h>

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
    const bento::Vector<checkup::TForecastInfo>& forecastInfoArray;
    bento::IAllocator& _allocator;

public:
    ReadyToGoScreen(SDL_Window* pwindow, int width, int height,
        const bento::Vector<checkup::TWeatherViewport>& weatherViewportArray,
        const bento::Vector<checkup::TWeatherInfo>& weatherInfoArray,
        const bento::Vector<checkup::TForecastInfo>& forecastInfoArray,
        bento::IAllocator& allocator);
    ~ReadyToGoScreen();

    virtual void draw(SDL_Renderer* renderer);
    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers);
};