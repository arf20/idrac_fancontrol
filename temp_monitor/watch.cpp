#include "watch.h"

#include <string>
#include <array>
#include <vector>
#include <chrono>
#include <cstdio>
#include <sstream>
#include <functional>
#include <memory>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

// Execute command, wait for it to finish and return its stdout
std::string exec(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

// Sensors
const std::string inletTempSens = 	"Inlet Temp       ";
const std::string exhaustTempSens = "Exhaust Temp     ";
const std::string cpuTempSens = 	"Temp             ";

const std::array<std::string, 6> fanSpeedSens { "Fan1             ", "Fan2             ", "Fan3             ", "Fan4             ", "Fan5             ", "Fan6             "};

const std::string totalPowerSens = 	"Pwr Consumption  ";

// IPMI tool command
const std::string ipmiCommand = "ipmitool sdr elist full";

// Watch
void watchLoop(std::function<void(SensorData)> callback) {
	while (true) {
		std::string ipmiout = exec(ipmiCommand);
		std::istringstream ss(ipmiout);
		std::string line;
		int cpuTempIdx = 0;
		// Deserialize stdout
        SensorData sd;
		while (std::getline(ss, line)) {
			const std::string sens = line.substr(0, 17);
			const std::string val = line.substr(38, std::string::npos);
			
			//std::cout << sens << ": " << val << ";" << std::endl; 
			
			if (sens == inletTempSens) sd.inletTemp = (float)std::stoi(val);
			if (sens == exhaustTempSens) sd.exhaustTemp = (float)std::stoi(val);
			if (sens == cpuTempSens) {
				sd.cpuTemps[cpuTempIdx] = (float)std::stoi(val);
				cpuTempIdx++;
			}
			for (int i = 0; i < fanSpeedSens.size(); i++) {
				if (sens == fanSpeedSens[i]) sd.fanSpeeds[i] = (float)std::stoi(val);
			}
			if (sens == totalPowerSens) sd.totalPower = (float)std::stoi(val);
		}

		sd.timeNow = std::chrono::system_clock::now();
		
		callback(sd);
	}
}
