#pragma once

#include <vector>
#include <array>
#include <chrono>

// Variables
extern std::chrono::time_point<std::chrono::system_clock> timeNow;

extern float inletTemp;
extern float exhaustTemp;
extern std::array<float, 2> cpuTemps;

extern std::array<float, 6> fanSpeeds;

extern float totalPower;

// History
extern int maxPoints;
extern std::vector<std::chrono::time_point<std::chrono::system_clock>> timeHist;

extern std::vector<float> inletTempHist;
extern std::vector<float> exhaustTempHist;
extern std::vector<std::array<float, 2>> cpuTempsHist;

extern std::vector<std::array<float, 6>> fanSpeedsHist;

extern std::vector<float> totalPowerHist;

// Graph functions
extern bool graphInit();
extern void graphLoop();