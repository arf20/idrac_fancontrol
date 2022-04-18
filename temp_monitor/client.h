#pragma once

#include <string>
#include <functional>

struct ControlData {
    bool control;
    float tempAvg;
    int ctrlSpeed;
};

extern bool clientInit(std::string srcAddr, unsigned short port);
extern void clientLoop(std::function<void(SensorData, ControlData)> callback);