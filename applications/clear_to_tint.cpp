// SDK includes
#include <bento_base/log.h>

// External includes
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_timer.h>
 
int main(int, char **)
{
    //  Init SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        bento::default_logger()->log(bento::LogLevel::error, "SDL Init", SDL_GetError());
    
    // Init window
    SDL_Window* win = SDL_CreateWindow("Clear To Tint", 0, 0, 1280, 720, 0);
 
    // creates a renderer to render our images
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
 
    // controls annimation loop
    int close = 0;
 

    // Set the clear color to black
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
 
    // annimation loop
    while (!close)
    {
        SDL_Event event;
 
        // Events management
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                case SDL_SCANCODE_F1:
                    close = 1;
                    break;
                default:
                    break;
            }
        }
       
        // clears the screen
        SDL_RenderClear(rend);
 
        // triggers the double buffers
        // for multiple rendering
        SDL_RenderPresent(rend);
 
        // calculates to 60 fps
        SDL_Delay(1000 / 60);
    }
 
    // destroy renderer
    SDL_DestroyRenderer(rend);
 
    // destroy window
    SDL_DestroyWindow(win);
     
    // close SDL
    SDL_Quit();
 
    return 0;
}