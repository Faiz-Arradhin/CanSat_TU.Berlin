#pragma once

#include "NetworkedModule.h"

ref class VC : public NetworkedModule {
public:
	VC(SM_ThreadManagement^ SM_TM, SM_VehicleControl^ control, SM_Port^ sm_port);
	error_state connect(String^ hostName, int portNumber) override;
	error_state communicate() override;
	error_state processSharedMemory() override;
	bool getShutdownFlag() override;
	void threadFunction() override;
	void disconnect();
	error_state processHeartbeats();
	void shutdownModules();

private:
	Byte packet;;

	array<Byte>^ buffer;
};