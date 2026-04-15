#pragma once

#include "NetworkedModule.h"

ref class Display : public NetworkedModule {
public:
	Display(SM_ThreadManagement^ SM_TM, SM_Sensors^ sm_lidar);
	error_state connect(String^ hostName, int portNumber) override;
	error_state communicate() override;
	error_state processSharedMemory() override;
	bool getShutdownFlag() override;
	void threadFunction() override;
	error_state processHeartbeats();
	void shutdownModules();
	void disconnect();

private:
	array<double>^ sensorData;

	UdpClient^ Udp;
	IPEndPoint^ RemoteEndPoint;

};