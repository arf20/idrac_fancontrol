#pragma once

#include <string>
#include <array>

// This example corresponds to my DELL PowerEdge R720, iDRAC7 2.61.60.60

#define CPU_N   2
#define FAN_N   6

// Sensors
const std::string inletTempSens = 	"Inlet Temp       ";
const std::string exhaustTempSens = "Exhaust Temp     ";
const std::string cpuTempSens = 	"Temp             ";        // Several instances with the same name, weird

const std::array<std::string, FAN_N> fanSpeedSens { "Fan1             ", "Fan2             ", "Fan3             ", "Fan4             ", "Fan5             ", "Fan6             "};

const std::string totalPowerSens = 	"Pwr Consumption  ";

// IPMI tool command
const std::string ipmiCommand = "ipmitool sdr elist full";