#pragma once

#include "conf.h"

#include <chrono>
#include <array>
#include <functional>

// Types
struct SensorData {
    std::chrono::time_point<std::chrono::system_clock> timeNow;

    float inletTemp;
    float exhaustTemp;
    std::array<float, CPU_N> cpuTemps;

    std::array<float, FAN_N> fanSpeeds;

    float totalPower;
};

// Functions
extern void watchLoop(std::function<void(SensorData)> callback);
