#include "../common/watch.h"
#include "../common/graph.h"

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <thread>
#include <chrono>

bool screenOverwrite = true;

void watchCallback(SensorData sd) {
	// Print
	#define PRINT
	#ifdef PRINT
	std::cout << "Temps:" << std::endl 
		<< "\tInlet: " << sd.inletTemp << "\tExhaust: " << sd.exhaustTemp << std::endl
		<< "\tCpu0: " << sd.cpuTemps[0] << "\tCpu1: " << sd.cpuTemps[1] << std::endl
	<< "Fan Speeds" << std::endl
		<< "\tFan1: " << sd.fanSpeeds[0] << "\tFan2: " << sd.fanSpeeds[1] << "\tFan3: " << sd.fanSpeeds[2] << std::endl
		<< "\tFan4: " << sd.fanSpeeds[3] << "\tFan5: " << sd.fanSpeeds[4] << "\tFan6: " << sd.fanSpeeds[5] << std::endl
	<< "Total Power Consumption" << std::endl
		<< "\tPower: " << sd.totalPower << "\t\t" << maxPoints << std::endl;
	#endif

	// Move cursor to top
	#ifdef PRINT
	if (screenOverwrite)
		for (int i = 0; i < 8; i++)
			std::cout << "\x1b[A";
	#endif

	// Push to history
	timeHist.push_back(sd.timeNow);
	inletTempHist.push_back(sd.inletTemp);
	exhaustTempHist.push_back(sd.exhaustTemp);
	cpuTempsHist.push_back(sd.cpuTemps);
	fanSpeedsHist.push_back(sd.fanSpeeds);
	totalPowerHist.push_back(sd.totalPower);

	if (timeHist.size() > maxPoints) {
		timeHist.erase(timeHist.begin());
		inletTempHist.erase(inletTempHist.begin());
		exhaustTempHist.erase(exhaustTempHist.begin());
		cpuTempsHist.erase(cpuTempsHist.begin());
		fanSpeedsHist.erase(fanSpeedsHist.begin());
		totalPowerHist.erase(totalPowerHist.begin());
	}
}

int main(int argc, char **argv) {
	bool graph = true;

	for (int i = 1; i < argc; i++) {
		if (std::string(argv[i]) == "--no-graph") graph = false;
		if (std::string(argv[i]) == "--no-vt100") screenOverwrite = false;
	}

	if (!graph) watchLoop(watchCallback);
	
	if (!graphInit()) exit(1);

	std::thread graphThread(graphLoop);
	graphThread.detach();

	watchLoop(watchCallback);

	return 0;
}
