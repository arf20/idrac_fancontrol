#pragma once

#include "conf.h"

#include <vector>
#include <chrono>
#include <array>

// History
extern int maxPoints;
extern std::vector<std::chrono::time_point<std::chrono::system_clock>> timeHist;

extern std::vector<float> inletTempHist;
extern std::vector<float> exhaustTempHist;
extern std::vector<std::array<float, CPU_N>> cpuTempsHist;

extern std::vector<std::array<float, FAN_N>> fanSpeedsHist;

extern std::vector<float> totalPowerHist;

// Graph functions
extern bool graphInit();
extern void graphLoop();
