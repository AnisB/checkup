// SDL GUI includes
#include <sdlgui/screen.h>
#include <sdlgui/window.h>

// SDK includes
#include <checkup_weather/weather.h>
#include <checkup_weather/forecast.h>

class ReadyToGoScreen : public sdlgui::Screen
{
private:
    std::map<std::string, SDL_Texture*> m_icons;

public:
    ReadyToGoScreen(SDL_Window* pwindow, int width, int height,
        const bento::Vector<checkup::TWeatherViewport>& weatherViewportArray,
        const bento::Vector<checkup::TWeatherInfo>& weatherInfoArray,
        const bento::Vector<checkup::TForecastInfo>& forecastInfoArray,
        bento::IAllocator& allocator);
    ~ReadyToGoScreen();

    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers);
    virtual void draw(SDL_Renderer* renderer);
};