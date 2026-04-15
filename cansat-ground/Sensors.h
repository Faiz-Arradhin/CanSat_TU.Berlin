#pragma once

#include "SMObjects.h"
#include "NetworkedModule.h"

enum class RxState {
	WAIT_HEADER_0,
	WAIT_HEADER_1,
	READ_PAYLOAD
};


ref class Sensors : public NetworkedModule {
public:
	Sensors(SM_ThreadManagement^ SM_TM, SM_Sensors^ sm_sensors, SM_Port^ sm_port);
	error_state connect(String^ hostName, int portNumber) override;
	error_state communicate() override;							// Communicate over TCP connection (includes any error checking required on data)
	error_state processSharedMemory() override;
	bool getShutdownFlag() override;
	void threadFunction() override;
	void disconnect();
	error_state processHeartbeats();
	void shutdownModules();

private:
	uint8_t computeChecksum(int len);
	array<Byte>^ buf;
	RxState rxState;
	int payloadIndex;

	Stopwatch^ speedwatch;
	double vx = 0.0, vy = 0.0, vz = 0.0;   // initial velocity (assume 0 at t0)
	double ax_prev = 0.0, ay_prev = 0.0, az_prev = 0.0;
	double t_prev = 0.0;
	bool firstSample = true;
	
	double bias_ax = 0, bias_ay = 0, bias_az = 0;
	double bias_ax_prev = 0, bias_ay_prev = 0, bias_az_prev = 0;
	double bias_sum_ax = 0, bias_sum_ay = 0, bias_sum_az = 0;
	int bias_count = 0;
	bool bias_ready = false;
};