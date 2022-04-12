#pragma once

#include <vector>
#include <array>

// Variables
extern float inletTemp;
extern float exhaustTemp;
extern std::array<float, 2> cpuTemps;

extern std::array<float, 6> fanSpeeds;

extern float totalPower;

// History
extern std::vector<float> inletTempHist;
extern std::vector<float> exhaustTempHist;
extern std::vector<std::array<float, 2>> cpuTempsHist;

extern std::vector<std::array<float, 6>> fanSpeedsHist;

extern std::vector<float> totalPowerHist;

// Graph functions
extern bool graphInit();
extern void graphLoop();