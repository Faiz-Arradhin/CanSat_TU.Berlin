#include "Controller.h"

Controller::Controller(SM_ThreadManagement^ SM_TM, SM_VehicleControl^ control) {
	SM_TM_ = SM_TM;
	VCon = control;
	VStopwatch = gcnew Stopwatch;
	state = gcnew typedKey;
}

bool Controller::IsDown(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

void Controller::threadFunction() {
	//while (!controller->IsConnected()) {
	//	Console::WriteLine("Controller not connected");
	//};
	SM_TM_->ThreadBarrier->SignalAndWait();

	int i = 0;
    bool exiting = false;
	while (!getShutdownFlag()) {
		stateAction = state->typing(
			IsDown(VK_UP),
			IsDown(VK_DOWN),
			IsDown(VK_RIGHT),
			IsDown(VK_LEFT),
			IsDown(VK_SPACE),
			IsDown(VK_TAB)
		);
		processSharedMemory();
		processHeartbeats();
		Thread::Sleep(30);
	}

	disconnect();
	Console::WriteLine("Controller Thread is terminated");
}


error_state Controller::processSharedMemory() {
	//controllerState action = controller->GetState();

	try {
		Monitor::Enter(VCon->lockObject);

		if (stateAction != SatAction::NoChange) {
			VCon->action = stateAction;
			VCon->update = true;
		} else {
			VCon->update = false;
		}

	} catch (Exception^ e) {
		return ERR_MEMORY_ACCESS;
	} finally {
		Monitor::Exit(VCon->lockObject);
	}
	if (stateAction != SatAction::NoChange) Console::WriteLine(stateAction);
	return SUCCESS;
}

void Controller::disconnect() {
	int i = 0;
	//if (controller != nullptr) {
	//	try {
	//		delete controller;
	//		Console::WriteLine("Controller Connection Closed");
	//		controller = nullptr;
	//	} catch (Exception^ e) {
	//		Console::WriteLine("Error Controller Closing: {0}", e->Message);
	//	}
	//} else {
	//	Console::WriteLine("No Active Controller Connection");
	//}
}

error_state Controller::processHeartbeats() {
	//check if the LiDAR bit in the hearbeat byte is low
	if ((SM_TM_->heartbeat & bit_CONTROLLER) == 0) {
		//put the LiDAR bit high
		SM_TM_->heartbeat |= bit_CONTROLLER;
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

bool Controller::getShutdownFlag() {
	return (SM_TM_->shutdown & bit_CONTROLLER);
}

void Controller::shutdownModules() {
	SM_TM_->shutdown = 0xFF;
	Console::WriteLine("Controller here");
}