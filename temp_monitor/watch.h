#pragma once

#include <chrono>
#include <array>
#include <functional>

// Types
struct SensorData {
    std::chrono::time_point<std::chrono::system_clock> timeNow;

    float inletTemp;
    float exhaustTemp;
    std::array<float, 2> cpuTemps;

    std::array<float, 6> fanSpeeds;

    float totalPower;
};

// Functions
extern void watchLoop(std::function<void(SensorData)> callback);
