#include "Display.h"

Display::Display(SM_ThreadManagement^ SM_TM, SM_Sensors^ sm_sensors) {
	SM_TM_ = SM_TM;
	VSensors = sm_sensors;
	VStopwatch = gcnew Stopwatch;
	sensorData = gcnew array<double>(11);
	SendData = gcnew array<unsigned char>(11 * sizeof(double));

	Udp = gcnew UdpClient();
	RemoteEndPoint = gcnew IPEndPoint(IPAddress::Parse("127.0.0.1"), Port::Display);
}

void Display::threadFunction() {
	while (connect(Address::Local, Port::Display) != SUCCESS) {};
	SM_TM_->ThreadBarrier->SignalAndWait();

	VStopwatch->Start();

	int i = 0;
	while (!getShutdownFlag()) {
		Console::WriteLine("Display Thread is running {0}", i++);

		if (processSharedMemory() != SUCCESS) continue;
		if (communicate() != SUCCESS) continue;
		processHeartbeats();

		Thread::Sleep(30);
	}
	disconnect();
	Console::WriteLine("Display Thread is terminated");
};

error_state Display::connect(String^ hostName, int portNumber) {
	return SUCCESS;
}

void Display::disconnect() {
	if (Udp != nullptr) {
		try {
			Udp->Close();
			Console::WriteLine("Display Connection Closed");
			Udp = nullptr;
		} catch (Exception^ e) {
			Console::WriteLine("Error Display Closing: {0}", e->Message);
		}
	} else {
		Console::WriteLine("No Active Display Connection");
	}
}

error_state Display::processSharedMemory() {
	try {
		Monitor::Enter(VSensors->lockObject);

		// Example: copy from your shared memory into sensorData[]
		sensorData[0] = VSensors->accX;
		sensorData[1] = VSensors->accY;
		sensorData[2] = VSensors->accZ;
		sensorData[3] = VSensors->angleX;
		sensorData[4] = VSensors->angleY;
		sensorData[5] = VSensors->angleZ;
		sensorData[6] = VSensors->tempIn;
		sensorData[7] = VSensors->tempOut;
		sensorData[8] = VSensors->alt;
		sensorData[9] = VSensors->g_load;
		sensorData[10] = VSensors->v_total;
		Buffer::BlockCopy(sensorData, 0, SendData, 0, SendData->Length);
	} catch (Exception^ e) {
		return ERR_MEMORY_ACCESS;
	} finally {
		Monitor::Exit(VSensors->lockObject);
	}

	return SUCCESS;
}

error_state Display::communicate() {
	try {
		Udp->Send(SendData, SendData->Length, RemoteEndPoint);
	} catch (Exception^ e) {
		return ERR_CONNECTION;
	}

	return SUCCESS;
}

error_state Display::processHeartbeats() {
	//check if the LiDAR bit in the hearbeat byte is low
	if ((SM_TM_->heartbeat & bit_DISPLAY) == 0) {
		//put the LiDAR bit high
		SM_TM_->heartbeat |= bit_DISPLAY;
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

bool Display::getShutdownFlag() {
	return SM_TM_->shutdown & bit_DISPLAY;
}

void Display::shutdownModules() {
	SM_TM_->shutdown = 0xFF;
}
