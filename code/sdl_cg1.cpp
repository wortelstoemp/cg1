#include <iostream>
#include <GL/glew.h>
#include <SDL.h>

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
	   std::cout << "SDL_Init didn't work!" << std::endl;
       return 1;
    }
    
    std::cout << "Hello World!" << std::endl;
    
    std::cin.get();
	return 0;
}
