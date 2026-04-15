#include "VC.h"

VC::VC(SM_ThreadManagement^ SM_TM, SM_VehicleControl^ control, SM_Port^ sm_port) {
	SM_TM_ = SM_TM;
	VCon = control;
	VPort = sm_port;
	VStopwatch = gcnew Stopwatch;

	buffer = gcnew array<Byte>(1);
}

void VC::threadFunction() {
	while (connect("", 0) != SUCCESS) {};
	SM_TM_->ThreadBarrier->SignalAndWait();

	VStopwatch->Start();

	int i = 0;
	while (!getShutdownFlag()) {
		//Console::WriteLine("VC Thread is running {0}     ", i++);
		processHeartbeats();

		if (processSharedMemory() != SUCCESS) continue;
		if (communicate() != SUCCESS) continue;

		Thread::Sleep(30);
	}

	disconnect();
	Console::WriteLine("VC Thread is terminated");
};

error_state VC::connect(String^ hostName, int portNumber) {
	try {
		if (!VPort->port->IsOpen) {
			VPort->connect();
		}
		return SUCCESS;
	} catch (Exception^ e) {
		Console::WriteLine("Connection VC Failed: {0}", e->Message);
		return ERR_CONNECTION;
	}
}

void VC::disconnect() {
	VPort->disconnect();
}

error_state VC::communicate() {
	try {
		VPort->port->Write(buffer, 0, 1);
	} catch (Exception^ e) {
		return ERR_INVALID_DATA;
	}

	return SUCCESS;
}

error_state VC::processSharedMemory() {
	String^ Command;
	unsigned char cmd = 0;
	try {
		Monitor::Enter(VCon->lockObject);
		if (!VCon->update) return ERR_NO_DATA;
		packet = (Byte)VCon->action & 0x0F;
		buffer[0] = packet;
	} catch (Exception^ e) {
		return ERR_MEMORY_ACCESS;
	} finally {
		Monitor::Exit(VCon->lockObject);
	}

	return SUCCESS;
}

error_state VC::processHeartbeats() {
	//check if the LiDAR bit in the hearbeat byte is low
	if ((SM_TM_->heartbeat & bit_VC) == 0) {
		//put the LiDAR bit high
		SM_TM_->heartbeat |= bit_VC;
		//reset the stopwatch
		VStopwatch->Restart();
	} else {
		//check if the time elasped exceed the crash time limit
		if (VStopwatch->ElapsedMilliseconds > CRASH_LIMIT) {
			//shut down all threads
			shutdownModules();
			return ERR_TMT_FAILURE;
		}
	}

	return SUCCESS;
}

void VC::shutdownModules() {
	Console::WriteLine("VC here");
	SM_TM_->shutdown = 0xFF;
}

bool VC::getShutdownFlag() {
	return SM_TM_->shutdown & bit_VC;
}
