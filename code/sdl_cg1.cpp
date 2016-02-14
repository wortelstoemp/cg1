#include <stdlib.h>
#include <iostream>

#include <GL/glew.h>
#include <SDL.h>

bool HandleEvent(SDL_Event* event);

int main(int argc, char *argv[])
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
	   std::cout << "Error: SDL could not initialize! " 
                 << SDL_GetError() << std::endl;
       std::cin.get();
       return 1;
    }
    
    // Initialize Window
    SDL_Window* window = SDL_CreateWindow("Computer Graphics",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          800,
                          600,
                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | 
                          SDL_WINDOW_INPUT_FOCUS);
    if (!window)
    {
        std::cout << "Could not create window: " 
                  << SDL_GetError() << std::endl;
        return 1;
    }
    
    // Initialize software renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer)
    {
        std::cout << "Could not create software renderer: " 
                  << SDL_GetError() << std::endl;
        return 1;
    }
    
    // Main loop
    bool isRunning = true;
    while(isRunning)
    {
        SDL_Event event;
        SDL_WaitEvent(&event);
        if (HandleEvent(&event))
        {
            isRunning = false;
        }
        
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    // Shutdown
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    
	return 0;
}

bool HandleEvent(SDL_Event* event)
{
    bool shouldQuit = false;
    switch(event->type)
    {
        case SDL_QUIT:
        {
            std::cout << "Quit" << std::endl;
            shouldQuit = true;
        } break;
        
        case SDL_WINDOWEVENT:
        {
            switch(event->window.event)
            {
                case SDL_WINDOWEVENT_RESIZED:
                {
                    std::cout << "Resized (" << event->window.data1 << ", "
                              << event->window.data2 << ")" << std::endl;
                } break;
                
                case SDL_WINDOWEVENT_EXPOSED:
                {
                    SDL_Window* window = SDL_GetWindowFromID(event->window.windowID);
                    SDL_Renderer* renderer = SDL_GetRenderer(window);
                    static bool isWhite = true;
                    if (isWhite)
                    {
                        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                        isWhite = false;
                    }
                    else
                    {
                        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                        isWhite = true;
                    }
                } break;
            }
        } break;
    }
    
    return shouldQuit;
}