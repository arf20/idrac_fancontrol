#include "main.h"

#include <iostream>

#include <SDL2/SDL.h>

#define WIDTH 1280
#define HEIGHT 720


bool graphInit() {
    SDL_Window *window;                     // window pointer
    SDL_Init(SDL_INIT_VIDEO);               // SDL2 init

    window = SDL_CreateWindow(
        "temp_monitor",                     // window title
        SDL_WINDOWPOS_UNDEFINED,            // initial x position
        SDL_WINDOWPOS_UNDEFINED,            // initial y position
        WIDTH,                              // width, in pixels
        HEIGHT,                             // height, in pixels
        SDL_WINDOW_OPENGL                   // flags - see below
    );

    if (window == NULL) {
        std::cout << "Error opening window: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}