#pragma once

#include <string>
#include <functional>

extern bool clientInit(std::string srcAddr, unsigned short port);
extern void clientLoop(std::function<void(SensorData)> callback);