#pragma once

#include "conf.h"

#include <chrono>
#include <array>
#include <functional>

// Functions
extern void watchLoop(std::function<void(SensorData)> callback);
