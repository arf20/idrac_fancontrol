#include "conf.h"
#include "graph.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// History
std::vector<std::chrono::time_point<std::chrono::system_clock>> timeHist;
std::vector<float> inletTempHist;
std::vector<float> exhaustTempHist;
std::vector<std::array<float, CPU_N>> cpuTempsHist;
std::vector<std::array<float, FAN_N>> fanSpeedsHist;
std::vector<float> totalPowerHist;


#define WIDTH 1280
#define HEIGHT 720

// Fonts
#define FONT "../FSEX300.ttf"
int midfontsize = 24;
int smallfontsize = 16;

// SDL stuff
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;

TTF_Font* midfont = nullptr;
TTF_Font* smallfont = nullptr;

#define TEXT_CENTERX    (unsigned int)1
#define TEXT_CENTERY    (unsigned int)2

SDL_bool done = SDL_FALSE;

// UI stuff
constexpr int margin = 20;

//constexpr int pointStep = 10;

constexpr int left = 5 * margin;
constexpr int right = WIDTH - (5 * margin);

constexpr float timeLimit = 180;  // seconds

//int maxPoints = ((right - left) / pointStep) + 1;

// Temperature box
constexpr int tempTop = (HEIGHT / 2) + margin;
constexpr int tempBottom = HEIGHT - (2 * margin);

constexpr int tempMin = 0;
constexpr int tempMax = 100;
constexpr float tempScaleStep = 5;

// Fan speed box
constexpr int fanTop = 3 * margin;
constexpr int fanBottom = (HEIGHT / 2) - margin;

constexpr int speedMin = 0;
constexpr int speedMax = 18000;
constexpr int speedScaleStep = 900;


SDL_Color White = {255, 255, 255};


// Logic stuff

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

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

     	
    if(TTF_Init() == -1) {
        std::cout << "Error initializing TTF API: " << TTF_GetError() << std::endl;
        return false;
    }

    // Text font
    if ((midfont = TTF_OpenFont(FONT, midfontsize)) == nullptr) {
        std::cout << "Error opening font: " << TTF_GetError() << std::endl;
        return false;
    }

    smallfont = TTF_OpenFont(FONT, smallfontsize); // Already checked file for good

    return true;
}

int DrawText(SDL_Renderer *renderer, std::string str, TTF_Font* font, int x, int y, unsigned int flags, SDL_Color color) {
    SDL_Surface* surfaceText = TTF_RenderText_Solid(font, str.c_str(), color);
    SDL_Texture* textureText = SDL_CreateTextureFromSurface(renderer, surfaceText);

    SDL_Rect rectText; //create a rect
    rectText.x = x;  //controls the rect's x coordinate 
    rectText.y = y; // controls the rect's y coordinte
    rectText.w = 0; // controls the width of the rect
    rectText.h = 0; // controls the height of the rect

    TTF_SizeText(font, str.c_str(), &rectText.w, &rectText.h);

    if (flags & TEXT_CENTERX) rectText.x -= rectText.w / 2;
    if (flags & TEXT_CENTERY) rectText.y -= rectText.h / 2;

    SDL_RenderCopy(renderer, textureText, nullptr, &rectText);

    // I had to run valgrind to find this, I'm such a terrible programmer
    SDL_FreeSurface(surfaceText);
    SDL_DestroyTexture(textureText);

    return 0;
}

int timeX(float time) {
    return (int)mapfloat(time, 0.0f, timeLimit, right, left);
}

void graphLoop() {
    while (!done) {
        SDL_Event event;

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // Draw temperature box
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawLine(renderer, left, tempTop, right, tempTop);
        SDL_RenderDrawLine(renderer, left, tempBottom, right, tempBottom);
        SDL_RenderDrawLine(renderer, left, tempTop, left, tempBottom);
        SDL_RenderDrawLine(renderer, right, tempTop, right, tempBottom);

        // Draw fan speed box
        SDL_RenderDrawLine(renderer, left, fanTop, right, fanTop);
        SDL_RenderDrawLine(renderer, left, fanBottom, right, fanBottom);
        SDL_RenderDrawLine(renderer, left, fanTop, left, fanBottom);
        SDL_RenderDrawLine(renderer, right, fanTop, right, fanBottom);

        // Draw temp scale
        SDL_SetRenderDrawColor(renderer, 64, 64, 64, SDL_ALPHA_OPAQUE);      // greyish
        int t = 0;
        for (int i = tempMin; i <= tempMax; i += tempScaleStep, t++) {
            int y = mapfloat(i, tempMin, tempMax, tempBottom, tempTop);
            if (t % 2 == 0)
                DrawText(renderer, std::to_string(i), smallfont, margin, y, TEXT_CENTERY, White);
            if (i != tempMin && i != tempMax)
                SDL_RenderDrawLine(renderer, left + 1, y, right - 1, y);
        }

        // Draw speed scale
        t = 0;
        for (int i = speedMin; i <= speedMax; i += speedScaleStep, t++) {
            int y = mapfloat(i, speedMin, speedMax, fanBottom, fanTop);
            if (t % 2 == 0) {
                std::stringstream stream;
                stream << std::fixed << std::setprecision(1) << (float)i / 1000.0f << " "  << std::setprecision(0) << ((float)i / (float)speedMax) * 100.0f;
                DrawText(renderer, stream.str(), smallfont, margin - 5, y, TEXT_CENTERY, White);
            }
            if (i != speedMin && i != speedMax)
                SDL_RenderDrawLine(renderer, left + 1, y, right - 1, y);
        }

        auto timeNow = std::chrono::system_clock::now();

        // Delete points older than timeLimit
        for (int i = 0; i < timeHist.size(); i++) {
            std::chrono::duration<float> diff = std::chrono::system_clock::now() - timeHist[i];
            if (diff.count() > timeLimit) {
                timeHist.erase(timeHist.begin());
                inletTempHist.erase(inletTempHist.begin());
                exhaustTempHist.erase(exhaustTempHist.begin());
                cpuTempsHist.erase(cpuTempsHist.begin());
                fanSpeedsHist.erase(fanSpeedsHist.begin());
                totalPowerHist.erase(totalPowerHist.begin());
            } else {
                break;
            }
        }

        // Draw time scale
        for (float t = 0; t < timeLimit; t += 5) {
            int x = timeX(t);
            //std::chrono::duration<float> diff = timeNow - timeHist[i];
            std::stringstream stream;
            stream << std::fixed << std::setprecision(0) << t;
            DrawText(renderer, stream.str(), smallfont, x, tempBottom + 3, TEXT_CENTERX, White);
            if (x != left && x != right) {
                SDL_RenderDrawLine(renderer, x, tempTop + 1, x, tempBottom - 1);
                SDL_RenderDrawLine(renderer, x, fanTop + 1, x, fanBottom - 1);
            }
        }

        // Draw units
        DrawText(renderer, "krpm %max", smallfont, margin - 5, fanTop - 25, 0, White);
        DrawText(renderer, "C", smallfont, margin, tempTop - 25, 0, White);
        DrawText(renderer, "s (ago)", smallfont, right + margin, tempBottom + 3, 0, White);

        // Draw graphs
        if (timeHist.size() > 1) {
            int size = timeHist.size();
            //int graphLeft = right - (size * pointStep) + pointStep;

            for (int i = 0; i < size - 1; i++) {
                std::chrono::duration<float> diff1 = std::chrono::system_clock::now() - timeHist[i];
                std::chrono::duration<float> diff2 = std::chrono::system_clock::now() - timeHist[i + 1];
                int x1 = timeX(diff1.count());
                int x2 = timeX(diff2.count());

                // Inlet
                int y1 = mapfloat(inletTempHist[i], tempMin, tempMax, tempBottom, tempTop);
                int y2 = mapfloat(inletTempHist[i + 1], tempMin, tempMax, tempBottom, tempTop);
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);

                // Exhaust
                y1 = mapfloat(exhaustTempHist[i], tempMin, tempMax, tempBottom, tempTop);
                y2 = mapfloat(exhaustTempHist[i + 1], tempMin, tempMax, tempBottom, tempTop);
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);

                // CPUs
                SDL_SetRenderDrawColor(renderer, 255, 0, 255, SDL_ALPHA_OPAQUE);
                for (int j = 0; j < CPU_N; j++) {
                    y1 = mapfloat(cpuTempsHist[i][j], tempMin, tempMax, tempBottom, tempTop);
                    y2 = mapfloat(cpuTempsHist[i + 1][j], tempMin, tempMax, tempBottom, tempTop);
                    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
                }

                // Fan speeds
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
                for (int j = 0; j < FAN_N; j++) {
                    y1 = mapfloat(fanSpeedsHist[i][j], speedMin, speedMax, fanBottom, fanTop);
                    y2 = mapfloat(fanSpeedsHist[i + 1][j], speedMin, speedMax, fanBottom, fanTop);
                    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
                }
            }
            
            // Inlet text
            int tempVal = inletTempHist.back();
            DrawText(renderer, "Inlet: " + std::to_string(tempVal), smallfont, right + 3, mapfloat(tempVal, tempMin, tempMax, tempBottom, tempTop), TEXT_CENTERY, {255, 0, 0});

            // Exhaust
            tempVal = exhaustTempHist.back();
            DrawText(renderer, "Exhaust: " + std::to_string(tempVal), smallfont, right + 3, mapfloat(tempVal, tempMin, tempMax, tempBottom, tempTop), TEXT_CENTERY, {0, 0, 255});

            // CPUs
            SDL_SetRenderDrawColor(renderer, 255, 0, 255, SDL_ALPHA_OPAQUE);
            for (int j = 0; j < CPU_N; j++) {
                tempVal = cpuTempsHist.back()[j];
                DrawText(renderer, "CPU " + std::to_string(j) + ": " + std::to_string(tempVal), smallfont, right + 3, mapfloat(tempVal, tempMin, tempMax, tempBottom, tempTop), TEXT_CENTERY, {255, 0, 255});
            }

            // Fan speeds
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
            for (int j = 0; j < FAN_N; j++) {
                tempVal = fanSpeedsHist.back()[j];
                DrawText(renderer, "Fan" + std::to_string(j) + ": " + std::to_string(tempVal), smallfont, right + 3, mapfloat(tempVal, speedMin, speedMax, fanBottom, fanTop), TEXT_CENTERY, {0, 255, 0});
            }
        }

        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = SDL_TRUE;
            }
        }
    }

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}
