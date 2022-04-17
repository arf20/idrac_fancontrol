#include "conf.h"

#include "watch.h"
#include "exec.h"

#include <string>
//#include <array>
//#include <vector>
#include <chrono>
#include <sstream>
#include <functional>

// Watch
void watchLoop(std::function<void(SensorData)> callback) {
	while (true) {
		std::string ipmiout = exec(ipmiCommand);
		std::istringstream ss(ipmiout);
		std::string line;
		int cpuTempIdx = 0;
		// Deserialize stdout
        SensorData sd { };
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
