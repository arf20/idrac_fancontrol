#include "../common/watch.h"
#include "../common/graph.h"
#include "../common/server.h"
#include "client.h"

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
bool client = false;

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
		<< "\tPower: " << sd.totalPower << std::endl;
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

	if (server) {
		// Send to multicast clients
		Packet packet { };

		// Set hostname
		char hostname[1024];
		gethostname(hostname, 1024);
		memcpy(packet.host, hostname, 24);
		packet.host[23] = '\0';

		packet.control = false;

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
}

void clientCallback(SensorData sd, ControlData cd) {
	graphRemote = true;

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
		<< "\tPower: " << sd.totalPower << std::endl;
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

	if (client && cd.control) {
		graphControl = true;
		graphTempAvg = cd.tempAvg;
		graphControlSpeed = cd.ctrlSpeed;
		ctrlSpeedHist.push_back(graphControlSpeed);
	}
}

int main(int argc, char **argv) {
	bool graph = true;
	client = false;

	for (int i = 1; i < argc; i++) {
		if (std::string(argv[i]) == "--no-graph") graph = false;
		if (std::string(argv[i]) == "--no-vt100") screenOverwrite = false;
		if (std::string(argv[i]) == "--server") { server = true; }
		if (std::string(argv[i]) == "--client")   client = true;
	}

	if (server)
		if (!serverInit(multicastAddr, multicastPort))
			exit(1);

	if (client)
		if (!clientInit(multicastAddr, multicastPort))
			exit(1);


	if (graph) { 
		if (!graphInit()) exit(1);

		std::thread graphThread(graphLoop);
		graphThread.detach();
	}
	

	if (!client)
		watchLoop(watchCallback);
	else
		clientLoop(clientCallback);

	return 0;
}
