#include "../common/conf.h"
#include "../common/watch.h"
#include "../common/graph.h"
#include "../common/server.h"

#include "control.h"

#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <cstring>

bool screenOverwrite = true;
bool server = false;

void watchCallback(SensorData sd) {
	// Push to history
	timeHist.push_back(sd.timeNow);
	inletTempHist.push_back(sd.inletTemp);
	exhaustTempHist.push_back(sd.exhaustTemp);
	cpuTempsHist.push_back(sd.cpuTemps);
	fanSpeedsHist.push_back(sd.fanSpeeds);
	totalPowerHist.push_back(sd.totalPower);

	if (server) {
		// Send to multicast clients
		Packet packet { };

		// Set hostname
		char hostname[1024];
		gethostname(hostname, 1024);
		memcpy(packet.host, hostname, 24);
		packet.host[23] = '\0';

		packet.timeNow = std::chrono::system_clock::to_time_t(sd.timeNow);

		// Set fields
		packet.inletTemp = sd.inletTemp;
		packet.exhaustTemp = sd.exhaustTemp;
		packet.totalPower = sd.totalPower;

		for (int i = 0; i < CPU_N; i++)
			packet.cpuTemps[i] = sd.cpuTemps[i];

		for (int i = 0; i < FAN_N; i++)
			packet.fanSpeeds[i] = sd.fanSpeeds[i];

		serverSend(packet);
	}

	// Compute SAMPLE_AVG_N sample average of the averages of CPU temperatures
	float tempAvg = 0.0f;
	if (timeHist.size() >= SAMPLE_AVG_N) {
		for (int i = timeHist.size() - 1; i > timeHist.size() - 1 - SAMPLE_AVG_N; i--) {
			float cpuAvg = 0.0f;
			for (int j = 0; j < CPU_N; j++)
				cpuAvg += cpuTempsHist[i][j];
			cpuAvg /= (float)CPU_N;
			tempAvg += cpuAvg;
		}
		tempAvg /= (float)SAMPLE_AVG_N;

		// If temp is in curve
		int speed = 0;
		if (tempAvg >= fanCurve[0].first && tempAvg <= fanCurve[fanCurve.size() - 1].first) {
			for (int i = 0; i < fanCurve.size() - 1; i++) {
				// Find segment of curve where temp is
				if (tempAvg >= fanCurve[i].first && tempAvg <= fanCurve[i + 1].first) {
					// Compute speed (y) from temp (x), using the two point line formula
					float x1 = fanCurve[i].first; float y1 = fanCurve[i].second;
					float x2 = fanCurve[i + 1].first; float y2 = fanCurve[i + 1].second;
					speed = (((y2 - y1) / (x2 - x1)) * (tempAvg - x1)) + y1;
					break;
				}
			}

			std::cout << tempAvg << "ºC: " << speed << "%" << std::endl;
			setSpeed(speed);
		} else {
			// Temperature not handled, return control to iDRAC
			returnControl();
		}
	}

	// Print
	//#define PRINT
	#ifdef PRINT
	std::cout << "Temps:" << std::endl 
		<< "\tInlet: " << sd.inletTemp << "\tExhaust: " << sd.exhaustTemp << std::endl
		<< "\tCpu0: " << sd.cpuTemps[0] << "\tCpu1: " << sd.cpuTemps[1] << std::endl
	<< "Fan Speeds" << std::endl
		<< "\tFan1: " << sd.fanSpeeds[0] << "\tFan2: " << sd.fanSpeeds[1] << "\tFan3: " << sd.fanSpeeds[2] << std::endl
		<< "\tFan4: " << sd.fanSpeeds[3] << "\tFan5: " << sd.fanSpeeds[4] << "\tFan6: " << sd.fanSpeeds[5] << std::endl
	<< "Total Power Consumption" << std::endl
		<< "\tPower: " << sd.totalPower << std::endl
	<< "Averaged CPU temp: " << tempAvg << std::endl;
	#endif

	// Move cursor to top
	#ifdef PRINT
	if (screenOverwrite)
		for (int i = 0; i < 9; i++)
			std::cout << "\x1b[A";
	#endif
}

int main(int argc, char **argv) {
	bool graph = true;

	for (int i = 1; i < argc; i++) {
		if (std::string(argv[i]) == "--no-graph") graph = false;
		if (std::string(argv[i]) == "--no-vt100") screenOverwrite = false;
		if (std::string(argv[i]) == "--server") { server = true; }
	}

	if (server)
		if (!serverInit(multicastAddr, multicastPort))
			exit(1);

	if (graph) { 
		if (!graphInit()) exit(1);

		std::thread graphThread(graphLoop);
		graphThread.detach();
	}

	acquireControl();

	watchLoop(watchCallback);

	returnControl();

	return 0;
}
