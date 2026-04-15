#include "TMM.h"

ThreadProperties::ThreadProperties(
	ThreadStart^ start, Action^ end, bool critical, uint8_t bit_id, String^ threadName
) : ThreadStart_(start), ThreadEnd_(end), Critical(critical), BitID(bit_id), ThreadName(threadName) {}

error_state ThreadManagement::setupSharedMemory() {
	try {
		SM_TM_ = gcnew SM_ThreadManagement;
		VSensors = gcnew SM_Sensors;
		VCon = gcnew SM_VehicleControl;
		VPort = gcnew SM_Port("COM10", 57600);

		Save_ = gcnew Save(VSensors);

		Sensors_ = gcnew Sensors(SM_TM_, VSensors, VPort);
		Controller_ = gcnew Controller(SM_TM_, VCon);
		VC_ = gcnew VC(SM_TM_, VCon, VPort);
		Display_ = gcnew Display(SM_TM_,VSensors);

		ThreadPropertiesList = gcnew array<ThreadProperties^> {
			gcnew ThreadProperties(gcnew ThreadStart(Sensors_, &Sensors::threadFunction), gcnew Action(Sensors_, &Sensors::disconnect), true, bit_SENSORS, "Sensors Thread"),
			gcnew ThreadProperties(gcnew ThreadStart(Controller_, &Controller::threadFunction), gcnew Action(Controller_, &Controller::disconnect), true, bit_CONTROLLER, "Controller Thread"),
			gcnew ThreadProperties(gcnew ThreadStart(VC_, &VC::threadFunction), gcnew Action(VC_, &VC::disconnect), false, bit_VC, "VC Thread"),
			gcnew ThreadProperties(gcnew ThreadStart(Display_, &Display::threadFunction), gcnew Action(Display_, &Display::disconnect), true, bit_DISPLAY, "Display Thread"),
		};

		ThreadList = gcnew array<Thread^>(ThreadPropertiesList->Length);

		SM_TM_->WatchList = gcnew array<Stopwatch^>(ThreadPropertiesList->Length);
		SM_TM_->ThreadBarrier = gcnew Barrier(ThreadPropertiesList->Length + 1);

		for (int i = 0; i < ThreadPropertiesList->Length; ++i) {
			SM_TM_->WatchList[i] = gcnew Stopwatch;
			ThreadList[i] = gcnew Thread(ThreadPropertiesList[i]->ThreadStart_);
		}
	} catch (Exception^ e) {
		return ERR_MEMORY_ACCESS;
	}

	return SUCCESS;
}

error_state ThreadManagement::processSharedMemory() {
	return Save_->write();
};

void ThreadManagement::threadFunction() {
	for (int i = 0; i < ThreadPropertiesList->Length; ++i) {
		ThreadList[i]->Start();
	}

	SM_TM_->ThreadBarrier->SignalAndWait();

	for (int i = 0; i < ThreadList->Length; ++i) {
		SM_TM_->WatchList[i]->Start();
	}

	int i = 0;
	while (!getShutdownFlag()) {
		if (Console::KeyAvailable && Console::ReadKey(true).KeyChar == 'q') break;
		processHeartbeats();
		processSharedMemory();
		Thread::Sleep(50);
	}

	shutdownModules();
	for (int i = 0; i < ThreadPropertiesList->Length; ++i) {
		ThreadList[i]->Join();
	}

	Console::WriteLine("TMT Thread is terminated\n\n\n\n");
}

//process heartbeat
error_state ThreadManagement::processHeartbeats() {
	//check the heartbeat flag of the ith thread (is it high?)
	for (int i = 0; i < ThreadList->Length; i++) {
		if (SM_TM_->heartbeat & ThreadPropertiesList[i]->BitID) {
			//if high, put the ith bit (flag) down
			SM_TM_->heartbeat &= ~ThreadPropertiesList[i]->BitID;
			//reset the stopwatch
			SM_TM_->WatchList[i]->Restart();
		} else {
			//check the stopwatch, is the time exceed the crash limit?
			if (SM_TM_->WatchList[i]->ElapsedMilliseconds > CRASH_LIMIT) {
				//is the ith thread a critical one?
				if (ThreadPropertiesList[i]->Critical) {
					Console::WriteLine(ThreadPropertiesList[i]->ThreadName + " (critical) failure. Shutting down all threads");
					shutdownModules();//shut down all
					return ERR_CRITICAL;
				} else {
					Console::WriteLine(ThreadPropertiesList[i]->ThreadName + " failed...Restarting/\/\/\/\/\/\//\\//\/\\//\/\/\/\//\/\/\/\//\/\/\/");
					ThreadList[i]->Abort();
					ThreadPropertiesList[i]->ThreadEnd_();
					ThreadList[i] = gcnew Thread(ThreadPropertiesList[i]->ThreadStart_);
					SM_TM_->ThreadBarrier = gcnew Barrier(1);

					SM_TM_->WatchList[i]->Restart();
					ThreadList[i]->Start();//try to restart
				}
			}
		}
	}

	return SUCCESS;
}

//shut down threads
void ThreadManagement::shutdownModules() {
	Console::WriteLine("TMM here");
	SM_TM_->shutdown = 0xFF; //0b1111 1111
}

//get shutdown flag
bool ThreadManagement::getShutdownFlag() {
	return (SM_TM_->shutdown & bit_TM);
}