#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <GL/glew.h> // Later for OpenGL
#include <SDL.h>

#include "math/line.h"

class SDLClock
{
private:
    Uint64  performanceFrequency;
    Uint64  current;
    Uint64  last;
    float   fixed;
    float   accumulator;

public:
    SDLClock(float fps)
    {
        this->performanceFrequency = SDL_GetPerformanceFrequency();
        this->current = SDL_GetPerformanceCounter();
        this->last = this->current;
        this->fixed = 1.0 / fps;
        this->accumulator = 0.0;
    }
    
    inline bool Accumulating() const
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
        this->last = SDL_GetPerformanceCounter();
    }
    
    inline float InterpolationAlpha() const
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

struct SDLWindowDimension
{
    int width;
    int height;
};

class SDLWindow
{
    friend class SDLRenderer;
private:
    SDL_Window* window;
    char* title;
    int width;
    int height;
    bool isFullscreen;
    
public:
    SDLWindow(char* title, int width, int height, bool isFullscreen)
        : title(title), width(width), height(height), isFullscreen(isFullscreen)
    {
    }
    
    bool Init()
    {
        atexit(SDL_Quit);
        if(SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
            return false;
        this->window = SDL_CreateWindow("Computer Graphics",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          this->width,
                          this->height,
                          SDL_WINDOW_SHOWN |
                          /*SDL_WINDOW_RESIZABLE |*/
                          (this->isFullscreen ? SDL_WINDOW_FULLSCREEN : 0) |  
                          SDL_RENDERER_PRESENTVSYNC |
                          SDL_WINDOW_INPUT_FOCUS);
        return (this->window != nullptr);
    }
    
    void Shutdown() const
    {
        SDL_DestroyWindow(this->window);
    }

    SDLWindowDimension GetWindowDimension() const
    {
        SDLWindowDimension result;
        SDL_GetWindowSize(this->window, &result.width, &result.height);
        return result;  
    }
};

class SDLBackBuffer
{
private:  
    SDL_Texture*      texture;
    unsigned char*    memory;
    int               width;
    int               height;
    int               pitch;
    int               bytesPerPixel;

public:  
    SDLBackBuffer(SDL_Renderer* renderer, const int width, const int height)
    {
        this->texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height);    
        if (!this->texture)
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
      
    ~SDLBackBuffer()
    {
        delete[] this->memory;
        SDL_DestroyTexture(texture);
    }
      
    inline int GetWidth() const { return this->width; }
    inline int GetHeight() const { return this->height; }
    
    inline void SetPixel(const int x, const int y, const Color& color)
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
        
		#pragma omp parallel for
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
    
    inline void SwapBuffers(SDL_Renderer* renderer) const
    {
      SDL_UpdateTexture(this->texture, 0, this->memory, this->pitch);
      SDL_RenderCopy(renderer, texture, 0, 0);
      SDL_RenderPresent(renderer);
    }
};

class SDLRenderer
{
private:
    SDLWindow*      window;
    SDL_Renderer*   renderer;
    SDLBackBuffer*  backbuffer;
	int*			scanbuffer;
    // TODO: scanbuffer? edgetable? ...
    
public:
    SDLRenderer(SDLWindow* window)
        : window(window)
    {
    }
    
    bool Init()
    {
        this->renderer = SDL_CreateRenderer(this->window->window, -1, SDL_RENDERER_SOFTWARE);
        const SDLWindowDimension dimension = this->window->GetWindowDimension();
        this->backbuffer = new SDLBackBuffer(this->renderer, dimension.width, dimension.height);
        this->scanbuffer = new int[dimension.height * 2];
		
		return (this->renderer != nullptr);
    }
    
    void Shutdown() const
    {
		delete this->scanbuffer;
        delete this->backbuffer;
        SDL_DestroyRenderer(this->renderer);
    }
    
    inline void SetPixel(const int x, const int y, const Color& color) const
    {
        const int width = this->backbuffer->GetWidth();
        const int height = this->backbuffer->GetHeight();
        
        const int xPos = x + (width / 2);
        const int yPos = (height / 2) - y;
        
		// TODO remove if clipping for lines and polygons works
        if (xPos > 0 && yPos > 0 && xPos < width && yPos < height)
        {
            this->backbuffer->SetPixel(xPos, yPos, color);
        }
    }
	
	inline void SetScanBuffer(const int y, const int xMin, const int xMax)
	{
		this->scanbuffer[y * 2] = xMin;
		this->scanbuffer[(y * 2) + 1] = xMax;	
	}
	
	void FillShape(const int yMin, const int yMax)
	{
		int xMin, xMax;
		for (int y = yMax; y >= yMin; y--)
		{
			xMin = scanbuffer[y * 2];
			xMax = scanbuffer[(y * 2) + 1];
			
			for(int x = xMin; x < xMax; x++)
			{
				this->SetPixel(x, y, Color{ 255, 255, 255, 255 });
			}		
		}
	}

    inline void Clear(const Color& color) const
    {
        this->backbuffer->Clear(color);
    }
    
    inline void Resize()
    {
        
    }
    
    inline void SwapBuffers() const
    {
        this->backbuffer->SwapBuffers(this->renderer);
    }
    
    void DrawLine(const Line& line, const Color& color)
    {
        this->DrawDDALine(line, color);
    }
    
    void DrawDDALine(const Line& line, const Color& color)
    {   
        float m = ((float)(line.y1 - line.y0)) / (line.x1 - line.x0);
        
        if (line.x0 <= line.x1)
        {
            if (m > 1)
            {
                m = 1.0f / m;
                float x = line.x0;
                for(int y = line.y0; y <= line.y1; ++y, x += m)
                    this->SetPixel((int) floor(x + 0.5), y, color);
            }
            else if (m < -1)
            {
                m = 1.0f / m;
                float x = line.x0;
                for(int y = line.y0; y >= line.y1; --y, x -= m)
                    this->SetPixel((int) floor(x - 0.5), y, color);
            }
            else
            {
                float y = line.y0;
                for(int x = line.x0; x <= line.x1; ++x, y += m)
                    this->SetPixel(x, (int) floor(y + 0.5), color);
            }
        }
        else
        {
            if (m > 1)
            {
                m = 1.0f / m;
                float x = line.x0;
                for(int y = line.y0; y >= line.y1; --y, x -= m)
                    this->SetPixel((int) floor(x - 0.5), y, color);
            }
            else if (m < -1)
            {
                m = 1.0f / m;
                float x = line.x0;
                for(int y = line.y0; y <= line.y1; ++y, x += m)
                    this->SetPixel((int) floor(x + 0.5), y, color);
            }
            else
            {
                float y = line.y1;
                for(int x = line.x1; x <= line.x0; ++x, y += m)
                    this->SetPixel(x, (int) floor(y + 0.5), color);    
            }
        }
        
        // From course:
        // const float m = ((float)(line.y1 - line.y0)) / (line.x1 - line.x0);
        // float y = line.y0;
        // 
        // for(int x = line.x0; x <= line.x1; ++x)
        // {
        //     this->SetPixel(x, (int) floor(y + 0.5), color);
        //     y += m;
        // }
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
    
    inline void DrawAllCirclePoints(const int xMid, const int yMid, const int x, const int y, const Color& color)
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
    
    void DrawMidPointCircle(const int xMid, const int yMid, const int radius, const Color& color)
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
    
    void DrawSecondOrderMidPointCircle(const int xMid, const int yMid, const int radius, const Color& color)
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
    // Initialize Window
    SDLWindow* window = new SDLWindow("Computer Graphics", 800, 600, false);
    if (!window->Init())
    {
        std::cout << "Could not create window." << std::endl;
        delete window;
        return 1;
    }
    
    // Initialize software renderer
    SDLRenderer* renderer = new SDLRenderer(window);
    if (!renderer->Init())
    {
        std::cout << "Could not create renderer." << std::endl;
        delete renderer;
        window->Shutdown();
        delete window;
        return 1;
    }
    
    // Main loop
    bool isRunning = true;
    float dt = 0.0;
    SDLClock clock(60);
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
        
        renderer->Clear(Color{ 0, 0, 0, 255 });
        //renderer->SetPixel(-100, 100, Color{ 255, 255, 255, 255 });
        const float radius = 100.0f;
        static float radians = 0.0f;
        const Line line(0, 0, radius * cos(radians), radius * sin(radians));
        renderer->DrawLine(line, Color{ 0, 255, 255, 255 });
        radians += dt * 0.05f;
        radians = radians >= 360.0f ? 0.0f : radians;     
        // renderer->DrawMidPointCircle(200, 200, 20, Color{ 255, 255, 0, 255 });
        // renderer->DrawSecondOrderMidPointCircle(0, 0, 100, Color{ 255, 0, 255, 255 });        
        
		for (int y = 100; y < 200; y++)
		{
			renderer->SetScanBuffer(y, -200, -100);
		}
		
		renderer->FillShape(100, 200);
		
		renderer->SwapBuffers();
    }

    // Shutdown
    renderer->Shutdown();
    delete renderer;
    window->Shutdown();
    delete window;
    
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