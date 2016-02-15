#include <stdlib.h>
#include <iostream>

#include <GL/glew.h> // Later for OpenGL
#include <SDL.h>

struct Color
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class BackBuffer
{
 private:
      SDL_Renderer*     renderer;   
      SDL_Texture*      texture;
      unsigned char*    memory;
      int               width;
      int               height;
      int               pitch;
      int               bytesPerPixel;

public:  
      BackBuffer(SDL_Renderer* renderer, const int width, const int height)
      {
          this->renderer = renderer;
          this->texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height);
    
          if(!texture)
          {
            std::cout << "Could not create frontbuffer: " 
                  << SDL_GetError() << std::endl;
          }
          this->bytesPerPixel = 4;
          this->pitch = width * bytesPerPixel;
          this->memory = new unsigned char[this->pitch * height];
          this->width = width;
          this->height = height;
      }
      
      ~BackBuffer()
      {
          delete[] this->memory;
          SDL_DestroyTexture(texture);
      }
      
      inline void SetPixel(int x, int y, const Color& color)
      {
          const int index = (x + y*this->width) * this->bytesPerPixel;
          this->memory[index] = color.b;
          this->memory[index + 1] = color.g;
          this->memory[index + 2] = color.r;
          this->memory[index + 3] = color.a;          
      }
      
      void Clear(const Color& color)
      {
          const int columns = this->width * this->bytesPerPixel;
          const int rows = this->height;
          
          for (int i = 0; i < rows; ++i)
          {
              for (int j = 0; j < columns; j += this->bytesPerPixel)
              {
                  // Format: BGRA
                  const int index = i*columns + j;
                  this->memory[index] = color.b;
                  this->memory[index + 1] = color.g;
                  this->memory[index + 2] = color.r;
                  this->memory[index + 3] = color.a;  
              }
          }
      }
      
      void Resize()
      {
          
      }
      
      inline void SwapBuffers()
      {
        // Nodig? //this->pitch = this->width * this->bytesPerPixel;
        if (SDL_UpdateTexture(this->texture, 0, this->memory, this->pitch))
            std::cout << "Could not update texture." << std::endl;
        
        SDL_RenderCopy(this->renderer, texture, 0, 0);
        SDL_RenderPresent(this->renderer);
      }
};

class Canvas
{
private:
    BackBuffer* backbuffer;
    // TODO: scanbuffer? edgetable? ...
    
public:
    Canvas(SDL_Renderer* renderer, const int width, const int height)
        : backbuffer(new BackBuffer(renderer, width, height))
    {
    }
    
    ~Canvas()
    {
        delete backbuffer;
    }
    
    inline void SetPixel(int x, int y, const Color& color)
    {
        this->backbuffer->SetPixel(x, y, color);         
    }

    inline void Clear(const Color& color)
    {
        this->backbuffer->Clear(color);
    }
    
    inline void Resize()
    {
        
    }
    
    inline void SwapBuffers()
    {
        this->backbuffer->SwapBuffers();
    }
};


bool HandleEvent(const SDL_Event& event);

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
                          SDL_WINDOW_SHOWN | /*SDL_WINDOW_RESIZABLE |*/ 
                          SDL_WINDOW_INPUT_FOCUS);
    if (!window)
    {
        std::cout << "Could not create window: " 
                  << SDL_GetError() << std::endl;
        return 1;
    }
    
    int width = 0, height = 0;
    SDL_GetWindowSize(window, &width, &height);
    
    // Initialize software renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    
    if (!renderer)
    {
        std::cout << "Could not create software renderer: " 
                  << SDL_GetError() << std::endl;
        return 1;
    }
    
    Canvas* canvas = new Canvas(renderer, width, height);
    
    // Main loop
    bool isRunning = true;
    while(isRunning)
    {
        SDL_Event event;
        SDL_WaitEvent(&event);
        isRunning = HandleEvent(event);
        
        canvas->Clear(Color{ 0, 0, 0, 255 });
        canvas->SetPixel(400, 300, Color{ 255, 255, 255, 255 });
        canvas->SwapBuffers();
    }

    // Shutdown
    delete canvas;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
	return 0;
}

bool HandleEvent(const SDL_Event& event)
{
    bool isRunning = true;
    switch(event.type)
    {
        case SDL_QUIT:
        {
            std::cout << "Quit" << std::endl;
            isRunning = false;
        } break;
        
        case SDL_WINDOWEVENT:
        {
            switch(event.window.event)
            {
                case SDL_WINDOWEVENT_RESIZED:
                {
                    std::cout << "Resized (" << event.window.data1 << ", "
                              << event.window.data2 << ")" << std::endl;
                } break;
                
                case SDL_WINDOWEVENT_EXPOSED:
                {
                    SDL_Window* window = SDL_GetWindowFromID(event.window.windowID);
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
    
    return isRunning;
}