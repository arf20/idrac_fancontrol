#include "main.h"

#include <iostream>

#include <SDL2/SDL.h>

// History
std::vector<float> inletTempHist;
std::vector<float> exhaustTempHist;
std::vector<std::array<float, 2>> cpuTempsHist;
std::vector<std::array<float, 6>> fanSpeedsHist;
std::vector<float> totalPowerHist;

#define WIDTH 1280
#define HEIGHT 720

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

SDL_bool done = SDL_FALSE;

bool graphInit() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Error initializing SDL2: " << SDL_GetError() << std::endl;
    }

    // Create window
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

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

    return true;
}

void graphLoop() {
    int margin = 20;
    int tempTop = (HEIGHT / 2) + margin;
    int tempBottom = HEIGHT - margin;
    int tempLeft = 0 + margin;
    int tempRight = WIDTH - margin;

    while (!done) {
        SDL_Event event;

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // Draw temperature box
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(renderer, tempLeft, tempTop, tempRight, tempTop);
        SDL_RenderDrawLine(renderer, tempLeft, tempBottom, tempRight, tempBottom);
        SDL_RenderDrawLine(renderer, tempLeft, tempTop, tempLeft, tempBottom);
        SDL_RenderDrawLine(renderer, tempRight, tempTop, tempRight, tempBottom);

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = SDL_TRUE;
            }
        }
    }

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}