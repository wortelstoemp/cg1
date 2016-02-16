#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <GL/glew.h> // Later for OpenGL
#include <SDL.h>

#include "math/line.h"

class Clock
{
private:
    Uint64  performanceFrequency;
    Uint64  current;
    Uint64  last;
    float   fixed;
    float   accumulator;

public:
    Clock(float fps)
    {
        this->performanceFrequency = SDL_GetPerformanceFrequency();
        this->current = SDL_GetPerformanceCounter();
        this->last = this->current;
        this->fixed = 1.0 / fps;
        this->accumulator = 0.0;
    }
    
    inline bool Accumulating()
    {
        return (this->accumulator >= this->fixed);
    }
    
    inline void Accumulate()
    {
        this->accumulator -= this->fixed;
    }
    
    inline float Delta()
    {
        this->performanceFrequency = SDL_GetPerformanceFrequency();
        this->current = SDL_GetPerformanceCounter();
        Uint64 counterDelta = this->current - this->last;

        float delta = (((1000.0f * (float) counterDelta) / (float) performanceFrequency));
        if (delta > this->fixed)
            delta = this->fixed;
        float fps = (float) performanceFrequency / (float) counterDelta;

        //printf("%.10f ms/f, %.10f f/s\n", delta, fps);
        this->last = this->current;
        this->accumulator += delta;
        
        return delta;
    }
    
    inline void Start()
    {
        this->last =  SDL_GetPerformanceCounter();
    }
    
    inline float InterpolationAlpha()
    {
        return (this->accumulator / this->fixed);
    }
};

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
      
      inline int GetWidth() { return this->width; }
      inline int GetHeight() { return this->height; }
      
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
        if (SDL_UpdateTexture(this->texture, 0, this->memory, this->pitch))
        {
            std::cout << "Could not update texture." << std::endl;
            return;   
        }
        
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
        const int width = this->backbuffer->GetWidth();
        const int height = this->backbuffer->GetHeight();
        
        const int xPos = x + (width / 2);
        const int yPos = (height / 2) - y;
        
        if (xPos > 0 && yPos > 0 && xPos < width && yPos < height)
        {
            this->backbuffer->SetPixel(xPos, yPos, color);
        }
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
    
    
    void DrawDDALine(const Line& line, const Color& color)
    {
        // From course:
        const float m = ((float)(line.y1 - line.y0)) / (line.x1 - line.x0);
        float y = line.y0;
        
        for(int x = line.x0; x <= line.x1; ++x)
        {
            this->SetPixel(x, (int) floor(y + 0.5), color);
            y += m;
        }
    }
    
    void DrawMidPointLine(const Line& line, const Color& color)
    {
        // From course:
        int dy = line.y1 - line.y0;
        int dx = line.x1 - line.x0;        
        int d = dy * 2 - dx;
        int incrE = dy * 2;
        int incrNE = (dy - dx) * 2;
        int y = line.y0;
        
        for(int x = line.x0; x <= line.x1; ++x)
        {
            if (d <= 0)
            {
                d += incrE;
            }
            else
            {
                d += incrNE;
                y++;    
            }
            this->SetPixel(x, y, color);
        }
    }
    
    void DrawAllCirclePoints(int xMid, int yMid, int x, int y, const Color& color)
    {
       this->SetPixel(xMid + x, yMid + y, color);
	   this->SetPixel(xMid + y, yMid + x, color);
	   this->SetPixel(xMid + y, yMid - x, color);
	   this->SetPixel(xMid + x, yMid - y, color);
	   this->SetPixel(xMid - x, yMid - y, color);
	   this->SetPixel(xMid - y, yMid - x, color);
	   this->SetPixel(xMid - y, yMid + x, color);
	   this->SetPixel(xMid - x, yMid + y, color);   
    }
    
    void DrawMidPointCircle(int xMid, int yMid, int radius, const Color& color)
    {
        int d = 1 - radius;
        int y = radius;
        
        for (int x = 0; x < y; ++x)
        {
            if (d < 0)
            {
                d += x * 2 + 3;
            }
            else
            {
                d += (x - y) * 2 + 5;
                y--;      
            }
            
            DrawAllCirclePoints(xMid, yMid, x, y, color); 
        }
    }
    
    void DrawSecondOrderMidPointCircle(int xMid, int yMid, int radius, const Color& color)
    {
        int d = 1 - radius;
        int y = radius;
        int deltaE = 3;
        int deltaSE = 5 - radius * 2;
        
        for (int x = 0; x < y; ++x)
        {
            if (d < 0)
            {
                // Select E
                d += deltaE;
                deltaE += 2;
                deltaSE += 2;
            }
            else
            {
                // Select SE
                d += deltaSE;
                deltaE += 2;
                deltaSE += 4;
                y--;      
            }
            
            DrawAllCirclePoints(xMid, yMid, x, y, color); 
        }
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
                          SDL_WINDOW_SHOWN |
                          /*SDL_WINDOW_RESIZABLE |*/ 
                          SDL_RENDERER_PRESENTVSYNC |
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
    float dt = 0.0;
    Clock clock(60);
    clock.Start();
    
    while(isRunning)
    {
        dt = clock.Delta();
        
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            isRunning = HandleEvent(event);            
        }
        
        while (clock.Accumulating())
        {
            // Fixed stuff
            clock.Accumulate();
        }
        
        canvas->Clear(Color{ 0, 0, 0, 255 });
        canvas->SetPixel(-100, 100, Color{ 255, 255, 255, 255 });
        Line line(-200, -200, -100, -100);
        canvas->DrawMidPointLine(line, Color{ 0, 255, 255, 255 });        
        canvas->DrawMidPointCircle(200, 200, 20, Color{ 255, 255, 0, 255 });
        canvas->DrawSecondOrderMidPointCircle(0, 0, 100, Color{ 255, 0, 255, 255 });        
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