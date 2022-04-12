#include "main.h"

#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <thread>
#include <chrono>

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

// Variables
float inletTemp = 0.0f;
float exhaustTemp = 0.0f;
std::array<float, 2> cpuTemps = { 0.0f, 0.0f};

std::array<float, 6> fanSpeeds = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

float totalPower = 0.0f;

// Sensors
const std::string inletTempSens = 	"Inlet Temp       ";
const std::string exhaustTempSens = "Exhaust Temp     ";
const std::string cpuTempSens = 	"Temp             ";

const std::vector<std::string> fanSpeedSens = { "Fan1             ", "Fan2             ", "Fan3             ", "Fan4             ", "Fan5             ", "Fan6             "};

const std::string totalPowerSens = 	"Pwr Consumption  ";

// IPMI tool command
const std::string ipmiCommand = "ipmitool sdr elist full";


bool screenOverwrite = true;

// Watch
int watchLoop() {
	while (true) {
		std::string ipmiout = exec(ipmiCommand);
		std::istringstream ss(ipmiout);
		std::string line;
		int cpuTempIdx = 0;
		// Deserialize stdout
		while (std::getline(ss, line)) {
			const std::string sens = line.substr(0, 17);
			const std::string val = line.substr(38, std::string::npos);
			
			//std::cout << sens << ": " << val << ";" << std::endl; 
			
			if (sens == inletTempSens) inletTemp = (float)std::stoi(val);
			if (sens == exhaustTempSens) exhaustTemp = (float)std::stoi(val);
			if (sens == cpuTempSens) {
				cpuTemps[cpuTempIdx] = (float)std::stoi(val);
				cpuTempIdx++;
			}
			for (int i = 0; i < fanSpeedSens.size(); i++) {
				if (sens == fanSpeedSens[i]) fanSpeeds[i] = (float)std::stoi(val);
			}
			if (sens == totalPowerSens) totalPower = (float)std::stoi(val);
		}
		
		// Print
		std::cout << "Temps:" << std::endl 
			<< "\tInlet: " << inletTemp << "\tExhaust: " << exhaustTemp << std::endl
			<< "\tCpu0: " << cpuTemps[0] << "\tCpu1: " << cpuTemps[1] << std::endl
		<< "Fan Speeds" << std::endl
			<< "\tFan1: " << fanSpeeds[0] << "\tFan2: " << fanSpeeds[1] << "\tFan3: " << fanSpeeds[2] << std::endl
			<< "\tFan4: " << fanSpeeds[3] << "\tFan5: " << fanSpeeds[4] << "\tFan6: " << fanSpeeds[5] << std::endl
		<< "Total Power Consumption" << std::endl
			<< "\tPower: " << totalPower << std::endl;

		// Move cursor to top
		if (screenOverwrite)
			for (int i = 0; i < 8; i++)
				std::cout << "\x1b[A";

		// Push to history
		inletTempHist.push_back(inletTemp);
		exhaustTempHist.push_back(exhaustTemp);
		cpuTempsHist.push_back(cpuTemps);
		fanSpeedsHist.push_back(fanSpeeds);
		totalPowerHist.push_back(totalPower);
	}
}


int main(int argc, char **argv) {
	bool graph = true;

	for (int i = 1; i < argc; i++) {
		if (std::string(argv[i]) == "--no-graph") graph = false;
		if (std::string(argv[i]) == "--no-vt100") screenOverwrite = false;
	}

	if (!graph) watchLoop();
	
	if (!graphInit()) exit(1);

	std::thread graphThread(graphLoop);
	graphThread.detach();

	watchLoop();

	return 0;
}
