#pragma once

#include <string>
#include <array>
#include <vector>
#include <chrono>

// This example corresponds to my DELL PowerEdge R720, iDRAC7 2.61.60.60

#define CPU_N   2
#define FAN_N   6

// Sensors
const std::string inletTempSens = 	"Inlet Temp       ";
const std::string exhaustTempSens = "Exhaust Temp     ";
const std::string cpuTempSens = 	"Temp             ";        // Several instances with the same name, weird

const std::array<std::string, FAN_N> fanSpeedSens { "Fan1             ", "Fan2             ", "Fan3             ", "Fan4             ", "Fan5             ", "Fan6             "};

const std::string totalPowerSens = 	"Pwr Consumption  ";

// IPMI tool commands
const std::string ipmiCommand = "ipmitool sdr elist full";                          // Returns interesting data
const std::string ipmiSetSpeedCommand = "ipmitool raw 0x30 0x30 0x02 0xff 0x";      // Followed by fan speed percent in hex value
const std::string ipmiReturnControlCommand = "ipmitool raw 0x30 0x30 0x01 0x01";    // Enable dynamic fan control
const std::string ipmiAcquireControlCommand = "ipmitool raw 0x30 0x30 0x01 0x00";   // Disable dynamic fan control

struct SensorData {
    std::chrono::time_point<std::chrono::system_clock> timeNow;
    float inletTemp;
    float exhaustTemp;
    std::array<float, CPU_N> cpuTemps;
    std::array<float, FAN_N> fanSpeeds;
    float totalPower;
};

// CONTROL
#define SAMPLE_AVG_N	6   // Number of samples to average, to prevent speed spiking

// Fan curve
//      Temperatures must be from lower to higher
// TEMP     SPEED PERCENT
// 0ºC      0%
// 30ºC     0%
// 40ºC     10%
// 50ºC     15%
// 60ºC     30%
const std::vector<std::pair<float, float>> fanCurve = {
    {0.0f, 0.0f},
    {30.0f, 0.0f},
    {40.0f, 5.0f},
    {50.0f, 15.0f},
    {60.0f, 30.0f}
};

// MULTICAST
struct Packet {
    char host[24];
    time_t timeNow;
    uint8_t inletTemp;
    uint8_t exhaustTemp;
    uint8_t totalPower;
    uint8_t cpuTemps[CPU_N];
    uint16_t fanSpeeds[FAN_N];
};

const std::string multicastAddr = "239.255.217.152";
const unsigned short multicastPort = 29001;